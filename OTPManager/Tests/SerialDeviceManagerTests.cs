//
// SerialDeviceManagerTests.cs
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
using OTPManagerApi;
using OTPManagerApi.Serial;


namespace Tests
{
	[TestFixture()]
	public class SerialDeviceManagerTests
	{
		private static volatile OTPDeviceState lastState = OTPDeviceState.Disconnected;
		private static volatile Exception lastException = null;

		private static void OnDeviceManagerEvent(object sender, OTPDeviceEventArgs args)
		{
			lastState = args.State;
			lastException = args.Error;
		}

		private static void Reset()
		{
			lastState = OTPDeviceState.Disconnected;
			lastException = null;
		}

		private readonly static string portName = (((int)Environment.OSVersion.Platform == 4) || ((int)Environment.OSVersion.Platform == 6) || ((int)Environment.OSVersion.Platform == 128)) ?
			"/dev/ttyUSB0" : "COM1";

		[Test()]
		public void Connect()
		{
			Reset();
			var manager = new SerialDeviceManager(portName);
			manager.DeviceEvent.Subscribe(OnDeviceManagerEvent);
			try
			{
				manager.Connect();
				Assert.AreEqual(OTPDeviceState.Connected, lastState);
				Assert.IsNull(lastException);
			}
			finally
			{
				try { manager.Dispose(); }
				catch (Exception) { }
			}
		}

		[Test()]
		public void Connect_WrongPort()
		{
			Reset();
			var manager = new SerialDeviceManager("deadbeef");
			manager.DeviceEvent.Subscribe(OnDeviceManagerEvent);
			try
			{
				Assert.Throws(typeof(System.IO.IOException), manager.Connect);
				Assert.AreEqual(OTPDeviceState.Failed, lastState);
				Assert.IsInstanceOf(typeof(System.IO.IOException), lastException);
			}
			finally
			{
				try { manager.Dispose(); }
				catch (Exception) { }
			}
		}
	}
}
