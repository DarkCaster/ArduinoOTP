//
// ResyncHelper.cs
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
using System.Threading.Tasks;
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Helpers
{
	internal sealed class ResyncHelper
	{
		private readonly ICommHelper commHelper;
		private readonly Random random;
		private readonly ProtocolConfig config;

		public ResyncHelper(ICommHelper commHelper, ProtocolConfig config)
		{
			this.commHelper = commHelper;
			this.random = new Random();
			this.config = config;
		}

		public async Task Resync()
		{
			var reqBuff = new byte[commHelper.MaxPayloadSize];
			//send empty resync request
			await commHelper.SendRequest(ReqType.Resync, reqBuff, 0, 0);
			//read and drop answers until timeout exception, or data limit was reached
			var dataDropLeft = config.RESYNC_DATA_DROP_LIMIT;
			while(dataDropLeft>0)
			{
				try
				{
					var answer = await commHelper.ReceiveAnswer();
					if (answer.plLen > 0)
						dataDropLeft -= answer.plLen;
					else
						dataDropLeft -= 1;
				}
				catch (TimeoutException) { break; }
			}
			if (dataDropLeft <= 0)
				throw new Exception("Cannot confirm resync request transmisson!");
			//generate payload
			random.NextBytes(reqBuff);
			//send resync sequence
			await commHelper.SendRequest(ReqType.Resync, reqBuff, 0, reqBuff.Length);
			//receive resync answer
			var verification = await commHelper.ReceiveAnswer();
			//compare resync sequence
			if (verification.plLen != reqBuff.Length)
				throw new Exception("Resync verification sequence is incorrect!");
			for (int i = 0; i < verification.plLen; ++i)
				if (verification.payload[i] != reqBuff[i])
					throw new Exception("Resync verification sequence mismatch!");
			//send resync-complete request
			await commHelper.SendRequest(ReqType.ResyncComplete, reqBuff, 0, 0);
			//receive OK answer
			if ((await commHelper.ReceiveAnswer()).ansType != AnsType.Ok)
				throw new Exception("Final resync-confirmation failed!");
		}
	}
}
