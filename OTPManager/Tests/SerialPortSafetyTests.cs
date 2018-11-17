//
// SerialPortSafetyTests.cs
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
using NUnit.Framework;
using System;
using System.IO.Ports;
using System.Diagnostics;
using OTPManagerApi.Serial;
using DarkCaster.Async;

namespace Tests
{
	[TestFixture()]
	public class SerialPortSafetyTests
	{
		[Test()]
		public void ReadTimeout()
		{
			const int maxBytesDrop = 100; //drop this max number of bytes to clear port recv-buffer, if any
			const int timeoutErrorMS = 100;
			var config = new SerialProtocolConfig();
			using (var serial = new SerialPort(SerialDeviceManagerTests.portName, config.SERIAL_PORT_SPEED, Parity.None, 8, StopBits.One))
			{
				serial.ReadTimeout = config.CMD_TIMEOUT;
				serial.WriteTimeout = config.CMD_TIMEOUT;
				serial.Open();
				var bytesLeft = maxBytesDrop;
				var sw = new Stopwatch();
				var tmpBuff = new byte[1];
				while (bytesLeft > 0)
				{
					bool timedOut = false;
					//start timer
					sw.Restart();
					//read byte
					try { bytesLeft -= serial.Read(tmpBuff, 0, 1); }
					catch (TimeoutException) { timedOut = true; }
					//stop timer
					sw.Stop();
					//if timed out - measure interval, exit
					if (timedOut)
					{
						Assert.LessOrEqual(Math.Abs(sw.ElapsedMilliseconds - config.CMD_TIMEOUT), timeoutErrorMS);
						break;
					}		
				}
				if (bytesLeft < 1)
					throw new Exception("Failed to establish valid test conditions! Aborting test!");
			}
		}

		[Test()]
		public void ReadTimeoutAsync()
		{
			const int maxBytesDrop = 100; //drop this max number of bytes to clear port recv-buffer, if any
			const int timeoutErrorMS = 100;
			var config = new SerialProtocolConfig();
			using (var asyncRunner = new AsyncRunner())
			using (var serial = new SerialPort(SerialDeviceManagerTests.portName, config.SERIAL_PORT_SPEED, Parity.None, 8, StopBits.One))
			{
				serial.ReadTimeout = config.CMD_TIMEOUT;
				serial.WriteTimeout = config.CMD_TIMEOUT;
				serial.Open();
				var bytesLeft = maxBytesDrop;
				var sw = new Stopwatch();
				var tmpBuff = new byte[1];
				while (bytesLeft > 0)
				{
					bool timedOut = false;
					//start timer
					sw.Restart();
					//read byte
					try { bytesLeft -= asyncRunner.ExecuteTask(() => serial.BaseStream.ReadAsync(tmpBuff, 0, 1)); }
					catch (TimeoutException) { timedOut = true; }
					//stop timer
					sw.Stop();
					//if timed out - measure interval, exit
					if (timedOut)
					{
						Assert.LessOrEqual(Math.Abs(sw.ElapsedMilliseconds - config.CMD_TIMEOUT), timeoutErrorMS);
						break;
					}
				}
				if (bytesLeft < 1)
					throw new Exception("Failed to establish valid test conditions! Aborting test!");
			}
		}
	}
}
