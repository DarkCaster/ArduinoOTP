//
// ProtocolConfig.cs
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
namespace OTPManagerApi.Protocol
{
	public abstract class ProtocolConfig
	{
		public int CMD_HDR_SIZE { get; protected set; }
		public int CMD_CRC_SIZE { get; protected set; }
		public int CMD_BUFF_SIZE { get; protected set; }
		public int CMD_TIMEOUT { get; protected set; }
		public int CMD_SIZE_MASK { get; protected set; }
		public int CMD_MAX_REMSZ { get; protected set; }
		public int CMD_MIN_REMSZ { get; protected set; }
		public int CMD_MAX_PLSZ { get; protected set; }
		public int ANS_ALL_MASK { get; protected set; }
		public int RESYNC_DATA_DROP_LIMIT { get; protected set; }
		public int PING_INTERVAL { get; protected set; }
	}
}
