//
// SerialProtocolConfig.cs
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
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Serial
{
	public class SerialProtocolConfig : ProtocolConfig
	{
		public int SERIAL_PORT_SPEED { get; protected set; }

		public SerialProtocolConfig()
		{
			CMD_HDR_SIZE = 1;
			CMD_CRC_SIZE = 1;
			CMD_BUFF_SIZE = 16; //1 byte - header, up to 14 bytes - payload, 1 byte - crc8 checksum
			CMD_TIMEOUT = 500; //timeout for reading command payload
			CMD_SIZE_MASK = 0x1F;
			CMD_MAX_REMSZ = 15;
			CMD_MIN_REMSZ = 1;
			CMD_MAX_PLSZ = 14;
			ANS_ALL_MASK = 0xE0;
			RESYNC_DATA_DROP_LIMIT = 200;
			PING_INTERVAL = 500;
			SERIAL_PORT_SPEED = 38400;
		}
	}
}