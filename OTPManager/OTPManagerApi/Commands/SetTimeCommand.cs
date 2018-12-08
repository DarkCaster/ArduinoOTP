//
// SetTimeCommand.cs
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
namespace OTPManagerApi.Commands
{
	public sealed class SetTimeCommand : IOTPCommand
	{
		private readonly DateTime localTime;
		private readonly int UTCOffset;

		public SetTimeCommand(DateTime localTime, int UTCOffset)
		{
			this.localTime = localTime;
			this.UTCOffset = UTCOffset;
			if (UTCOffset > 86401 || UTCOffset < -86401)
				throw new Exception("UTC offset is more than 24 hours!");
		}

		public byte CommandType => 1;

		public byte[] SerializedData
		{
			get
			{
				var buff = new byte[10];
				var pos = 0;
				buff[pos++] = (byte)localTime.Second;
				buff[pos++] = (byte)localTime.Minute;
				buff[pos++] = (byte)localTime.Hour;
				buff[pos++] = (byte)(((int)localTime.DayOfWeek)<<5|localTime.Day);
				buff[pos++] = (byte)localTime.Month;
				buff[pos++] = (byte)(localTime.Year & 0xFF);
				buff[pos++] = (byte)((localTime.Year >> 8) & 0xFF);
				var sign = UTCOffset >= 0 ? 0 : 1;
				var offset = UTCOffset >= 0 ? UTCOffset : -UTCOffset;
				buff[pos++] = (byte)(offset & 0xFF);
				buff[pos++] = (byte)((offset >> 8) & 0xFF);
				buff[pos++] = (byte)(((offset >> 16) & 0xFF) | sign << 7);
				return buff;
			}
		}
	}
}
