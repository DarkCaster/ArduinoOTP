//
// CommHelperBase.cs
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
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Threading.Tasks;
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Helpers
{
	public abstract class CommHelperBase
	{
		public abstract Task<Answer> ReceiveAnswer();
		public abstract Task SendRequest(ReqType reqType, uint seq, byte[] plBuff, int offset, int len);
		public abstract int MaxPayloadSize { get; }

		protected readonly ProtocolConfig config;

		protected CommHelperBase(ProtocolConfig config) => this.config = config;

		public async Task<KeyValuePair<uint,uint>> Resync()
		{
			var reqBuff = new byte[MaxPayloadSize];
			//send empty resync request
			await SendRequest(ReqType.Resync, 0, reqBuff, 0, 0);
			//read and drop answers until timeout exception, or data limit was reached
			var dataDropLeft = config.RESYNC_DATA_DROP_LIMIT;
			while (dataDropLeft > 0)
			{
				try
				{
					var answer = await ReceiveAnswer();
					if (answer.plLen > 0)
						dataDropLeft -= answer.plLen;
					else
						dataDropLeft -= 1;
				}
				catch (TimeoutException) { break; }
			}
			if (dataDropLeft <= 0)
				throw new Exception("Cannot confirm resync request transmisson!");
			//generate payload with random sequence
			uint seq = 0;
			using (var random = new RNGCryptoServiceProvider())
			{
				random.GetBytes(reqBuff);
				byte[] bSeq = new byte[4];
				random.GetBytes(bSeq);
				seq = unchecked((uint)(bSeq[0] | bSeq[1] << 8 | bSeq[2] << 16 | bSeq[3] << 24));
			}
			//init request LCG
			var reqGen = new LCGen(seq);
			//send resync sequence
			await SendRequest(ReqType.Resync, reqGen.GenerateValue(), reqBuff, 0, reqBuff.Length);
			//receive resync answer
			var verification = await ReceiveAnswer();
			if (verification.ansType != AnsType.Resync)
				throw new Exception("Incorrect answer received during resync verification");
			//compare resync sequence
			if (verification.plLen != reqBuff.Length)
				throw new Exception("Resync verification sequence is incorrect!");
			//init answer LCG
			var ansGen = new LCGen(verification.seq);
			for (int i = 0; i < verification.plLen; ++i)
				if (verification.payload[i] != reqBuff[i])
					throw new Exception("Resync verification sequence mismatch!");
			//get final request and answer LCG init values
			var reqSeq = reqGen.GenerateValue();
			var ansSeq = ansGen.GenerateValue();
			//send resync-complete request
			await SendRequest(ReqType.ResyncComplete, reqSeq, reqBuff, 0, 0);
			//receive OK answer
			var confirmation = await ReceiveAnswer();
			if (confirmation.ansType != AnsType.Ok)
				throw new Exception("Final resync-confirmation failed!");
			if (confirmation.seq != ansSeq)
				throw new Exception("Incorrect seqence number received in resync-confirmation!");
			return new KeyValuePair<uint, uint>(reqSeq, ansSeq);
		}
	}
}
