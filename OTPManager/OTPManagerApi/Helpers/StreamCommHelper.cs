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
	public sealed class StreamCommHelper : ICommHelper
	{
		private const int CMD_HDR_SIZE = 1;
		private const int CMD_CRC_SIZE = 1;
		private const int CMD_BUFF_SIZE = 16; //1 byte - header, up to 14 bytes - payload, 1 byte - crc8 checksum
		private const int CMD_TIMEOUT = 500; //timeout for reading command payload

		private const int CMD_SIZE_MASK = 0x1F;
		private const int CMD_MAX_REMSZ = 15;
		private const int CMD_MIN_REMSZ = 1;
		private const int CMD_MAX_PLSZ = 14;

		private const int ANS_ALL_MASK = 0xE0;

		private readonly Stream link;
		private readonly byte[] recvBuff;
		private readonly byte[] sendBuff;

		public StreamCommHelper(Stream link)
		{
			this.link = link;
			this.recvBuff = new byte[CMD_BUFF_SIZE];
			this.sendBuff = new byte[CMD_BUFF_SIZE];
		}

		public int MaxPayloadSize => CMD_MAX_PLSZ;

		public async Task<Answer> ReceiveAnswer()
		{
			try
			{
				int bRead = 0;
				using (var cts = new CancellationTokenSource())
				{
					cts.CancelAfter(CMD_TIMEOUT);
					bRead = await link.ReadAsync(recvBuff, 0, 1, cts.Token);
					if (bRead != 1)
						throw new NotSupportedException("Stream has reached EOF and cannot receive data");
				}
				int remSz = recvBuff[0] & CMD_SIZE_MASK;
				if (remSz < CMD_MIN_REMSZ || remSz > CMD_MAX_REMSZ)
					return Answer.Invalid;
				if(!Enum.TryParse<AnsType>((recvBuff[0] & CMD_SIZE_MASK).ToString(),out AnsType ans))
					return Answer.Invalid;
				switch (ans)
				{
					case AnsType.Pong:
						if (remSz > CMD_CRC_SIZE)
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
					cts.CancelAfter(CMD_TIMEOUT);
					while (rem > 0)
						rem -= await link.ReadAsync(recvBuff, CMD_HDR_SIZE + (remSz - rem), rem, cts.Token);
				}
				//verify CRC
				var testSz = CMD_HDR_SIZE + remSz - 1;
				if (recvBuff[testSz] != CRC8.Calculate(recvBuff, 0, testSz))
					return Answer.Invalid;
				return new Answer(ans, recvBuff, CMD_HDR_SIZE, remSz - CMD_CRC_SIZE);
			}
			catch (TaskCanceledException ex)
			{
				throw new TimeoutException("ReceiveAnswer was timed out", ex);
			}
		}

		public async Task SendRequest(ReqType reqType, byte[] plBuff, int offset, int len)
		{
			try
			{
				if (len > CMD_MAX_PLSZ)
					throw new ArgumentException("len > CMD_MAX_PLSZ", nameof(len));
				Buffer.BlockCopy(plBuff, offset, sendBuff, CMD_HDR_SIZE, len);
				sendBuff[0] = (byte)((int)reqType | (len + CMD_CRC_SIZE));
				//write crc
				int testLen = len + CMD_HDR_SIZE;
				sendBuff[testLen] = CRC8.Calculate(sendBuff, 0, testLen);
				//send data
				using (var cts = new CancellationTokenSource())
				{
					cts.CancelAfter(CMD_TIMEOUT);
					await link.WriteAsync(sendBuff, 0, testLen + CMD_CRC_SIZE, cts.Token);
				}
			}
			catch (TaskCanceledException ex)
			{
				throw new TimeoutException("SendRequest was timed out", ex);
			}
		}
	}
}
