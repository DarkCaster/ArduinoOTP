//
// DeviceManagerBase.cs
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
using System.Threading.Tasks;
using OTPManagerApi.Helpers;
using OTPManagerApi.Protocol;

namespace OTPManagerApi
{
	/// <summary>
	/// Common device manager logic, without any complex error-reporting and connection-maintaining stuff
	/// </summary>
	public abstract class DeviceManagerBase
	{
		protected readonly LCGen reqLCG = new LCGen(0);
		protected readonly LCGen ansLCG = new LCGen(0);

		//must be implemented by inherited class
		//using properties because this parameters maybe hard to initialize inplace in base construct
		protected abstract CommHelperBase CommHelper { get; }
		protected abstract ProtocolConfig Config { get; }

		//protected abstract ProtocolConfig Config;
		protected abstract void PreCommandCommit();
		protected abstract void PostCommandCommit();

		protected async Task ReceiveOkAnswer()
		{
			var answer=await CommHelper.ReceiveAnswer();
			if (answer.ansType != AnsType.Ok)
				throw new Exception($"Received non OK answer: {answer.ansType}");
			if (answer.seq != ansLCG.GenerateValue())
				throw new Exception("Sequence number mismatch in OK answer");
		}

		protected async Task<IOTPResponse> ExecuteCommand(IOTPCommand command)
		{
			//send Command-begin request with command type
			await CommHelper.SendRequest(ReqType.Command, reqLCG.GenerateValue(), new byte[1] { command.CommandType }, 0, 1);
			//receive OK answer
			await ReceiveOkAnswer();
			//transfer command-data
			var cmdData = command.SerializedData;
			var dataLeft = cmdData.Length;
			while (dataLeft > 0)
			{
				var dataLen = dataLeft;
				if (dataLen > Config.CMD_MAX_PLSZ)
					dataLen = Config.CMD_MAX_PLSZ;
				//send CommandData request
				await CommHelper.SendRequest(ReqType.CommandData, reqLCG.GenerateValue(), cmdData, cmdData.Length - dataLeft, dataLen);
				//receive OK answer
				await ReceiveOkAnswer();
				dataLeft -= dataLen;
			}
			PreCommandCommit();
			//send Command-commit request
			await CommHelper.SendRequest(ReqType.Command, reqLCG.GenerateValue(), new byte[1] { 0 }, 0, 1);
			//receive OK answer
			await ReceiveOkAnswer();
			PostCommandCommit();
			//request response from device
			await CommHelper.SendRequest(ReqType.DataRequest, reqLCG.GenerateValue(), new byte[1] { 0 }, 0, 1);
			//receive dataMarker answer, that contains information about pending response
			var answer = await CommHelper.ReceiveAnswer();
			if (answer.ansType != AnsType.DataMarker)
				throw new Exception($"Received non DataMarker answer: {answer.ansType}");
			if (answer.seq != ansLCG.GenerateValue())
				throw new Exception("Sequence number mismatch in DataMarker answer");
			var respType = answer.payload[0];
			//receive data fragments
			var totalResponseSz = 0;
			var responseFragments = new List<byte[]>();
			while (totalResponseSz > Config.MAX_RESPONSE_SZ)
			{
				//request response-data from device
				await CommHelper.SendRequest(ReqType.DataRequest, reqLCG.GenerateValue(), new byte[1] { 1 }, 0, 1);
				//receive answer, that contains data chunk
				answer = await CommHelper.ReceiveAnswer();
				if (answer.ansType != AnsType.Data)
					throw new Exception($"Received non Data answer: {answer.ansType}");
				if (answer.seq != ansLCG.GenerateValue())
					throw new Exception("Sequence number mismatch in DataMarker answer");
				//generate response=object
				if (answer.payload.Length == 0)
				{
					var serializedData = new byte[totalResponseSz];
					var serializedPos = 0;
					foreach (var frag in responseFragments)
					{
						Buffer.BlockCopy(frag, 0, serializedData, serializedPos, frag.Length);
						serializedPos += frag.Length;
					}
					return ResponseHelper.DeserializeResponse(respType, serializedData);
				}
				responseFragments.Add(answer.payload);
				totalResponseSz += answer.payload.Length;
			}
			throw new Exception("Response data size reached!");
		}
	}
}
