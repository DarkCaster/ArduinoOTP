//
// CommHelper.cs
//
// Author:
//       DarkCaster <dark.caster@outlook.com>
//
// Copyright (c) 2018
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

using System;
using System.IO;
using System.Threading.Tasks;
using System.Threading;
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Helpers
{
	public class StreamCommHelper : CommHelperBase
	{
		private readonly byte[] recvBuff;
		private readonly byte[] sendBuff;
		private readonly Stream link;

		public StreamCommHelper(Stream link, ProtocolConfig config) : base(config)
		{
			this.recvBuff = new byte[config.CMD_BUFF_SIZE];
			this.sendBuff = new byte[config.CMD_BUFF_SIZE];
			this.link = link;
		}

		public override int MaxPayloadSize => config.CMD_MAX_PLSZ;

		public override async Task<Answer> ReceiveAnswer()
		{
			try
			{
				int bRead = 0;
				using (var cts = new CancellationTokenSource())
				{
					cts.CancelAfter(config.CMD_TIMEOUT);
					bRead = await link.ReadAsync(recvBuff, 0, 1, cts.Token);
					if (bRead != 1)
						throw new NotSupportedException("Stream has reached EOF and cannot receive data");
				}
				int remSz = recvBuff[0] & config.CMD_SIZE_MASK;
				if (remSz < config.CMD_MIN_REMSZ || remSz > config.CMD_MAX_REMSZ)
					return Answer.Invalid;
				if(!Enum.TryParse<AnsType>((recvBuff[0] & config.ANS_ALL_MASK).ToString(),out AnsType ans))
					return Answer.Invalid;
				switch (ans)
				{
					case AnsType.Pong:
						if (remSz > config.CMD_CRC_SIZE)
							return Answer.Invalid;
						break;
					case AnsType.Ok:
					case AnsType.Resync:
						break;
					default:
						return Answer.Invalid;
				}
				var rem = remSz;
				using (var cts = new CancellationTokenSource())
				{
					cts.CancelAfter(config.CMD_TIMEOUT);
					while (rem > 0)
						rem -= await link.ReadAsync(recvBuff, config.CMD_HDR_SIZE + (remSz - rem), rem, cts.Token);
				}
				//verify CRC
				var testSz = config.CMD_HDR_SIZE + remSz - 1;
				if (recvBuff[testSz] != CRC8.Calculate(recvBuff, 0, testSz))
					return Answer.Invalid;
				return new Answer(ans, recvBuff, config.CMD_HDR_SIZE, remSz - config.CMD_CRC_SIZE);
			}
			catch (Exception ex) when (ex is TaskCanceledException || ex is TimeoutException)
			{
				throw new TimeoutException("ReceiveAnswer was timed out", ex);
			}
		}

		public override async Task SendRequest(ReqType reqType, uint seq, byte[] plBuff, int offset, int len)
		{
			try
			{
				if (len > config.CMD_MAX_PLSZ)
					throw new ArgumentException("len > CMD_MAX_PLSZ", nameof(len));
				//write sequence number
				int pos = config.CMD_HDR_SIZE;
				int seqLen = 0;
				if (seq > 0)
				{
					if (config.CMD_SEQ_SIZE == 4)
					{
						sendBuff[pos++] = (byte)(seq & 0xFF);
						sendBuff[pos++] = (byte)((seq >> 8) & 0xFF);
						sendBuff[pos++] = (byte)((seq >> 16) & 0xFF);
						sendBuff[pos++] = (byte)((seq >> 24) & 0xFF);
					}
					else
						throw new Exception("config.CMD_SEQ_SIZE != 4");
					seqLen = config.CMD_SEQ_SIZE;
				}
				else
					len = 0;
				//write payload
				Buffer.BlockCopy(plBuff, offset, sendBuff, pos, len);
				pos += len;
				sendBuff[0] = (byte)((int)reqType | (len + seqLen + config.CMD_CRC_SIZE));
				//write crc
				int testLen = len + seqLen + config.CMD_HDR_SIZE;
				sendBuff[testLen] = CRC8.Calculate(sendBuff, 0, testLen);
				//send data
				using (var cts = new CancellationTokenSource())
				{
					cts.CancelAfter(config.CMD_TIMEOUT);
					await link.WriteAsync(sendBuff, 0, testLen + config.CMD_CRC_SIZE, cts.Token);
				}
			}
			catch (TaskCanceledException ex)
			{
				throw new TimeoutException("SendRequest was timed out", ex);
			}
		}
	}
}
