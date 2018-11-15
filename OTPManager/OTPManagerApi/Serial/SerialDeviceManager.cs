//
// SerialDeviceManager.cs
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
using System.IO.Ports;
using System.Threading.Tasks;
using DarkCaster.Events;
using OTPManagerApi.Helpers;

namespace OTPManagerApi.Serial
{
	public sealed class SerialDeviceManager : DeviceManagerBase
	{
		private readonly ISafeEventCtrl<OTPDeviceEventArgs> deviceEventCtrl;
		private readonly string portName;

		//will be re-created after connect/disconnect
		private SerialPort port;
		private StreamCommHelper commHelper;
		private bool isDisposed = false;
		private bool isConnected = false;
		private PingWorker pingWorker = null;

		private readonly SerialProtocolConfig config;

		private SerialDeviceManager(string serialPortName)
		{
#if DEBUG
			deviceEventCtrl = new SafeEventDbg<OTPDeviceEventArgs>();
#else
			deviceEventCtrl = new SafeEvent<OTPDeviceEventArgs>();
#endif
			portName = serialPortName;
			config = new SerialProtocolConfig();
		}

		public override ISafeEvent<OTPDeviceEventArgs> DeviceEvent => (ISafeEvent<OTPDeviceEventArgs>)deviceEventCtrl;

		protected override CommHelperBase CommHelper => commHelper;

		public override async Task Connect()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			if (isConnected)
				throw new Exception("Already connected!");
			//create new serial port object
			port = new SerialPort(portName, ((SerialProtocolConfig)config).SERIAL_PORT_SPEED, Parity.None, 8, StopBits.One);
			try
			{
				port.Open();
				commHelper = new StreamCommHelper(port.BaseStream, config);
				await base.Connect(); //run base connection routine
			}
			catch (Exception)
			{
				try { port.Dispose(); port = null; commHelper = null; } //TODO: state change
				catch (Exception) { }
				throw;
			}
			//run ping worker thread
			pingWorker = new PingWorker(CommHelper, config.PING_INTERVAL, null); //TODO: fail callback
			isConnected = true;
		}

		public override Task Disconnect()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			if (!isConnected)
				throw new Exception("Not connected!");
			isConnected = false; //method will not be triggered more than once
			try
			{
				pingWorker.Dispose(); //kill ping worker thread (should not throw exceptions)
				port.Close(); //close serial port, will call dispose internally
			}
			catch (Exception)
			{
				//TODO state change, send notification
				throw;
			}
			finally
			{
				pingWorker = null;
				port = null;
				commHelper = null;
			}
			//TODO state change, send notification
			return Task.FromResult(true);
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (isDisposed)
					return;
				if (isConnected)
					Disconnect();
				isDisposed = true;
			}
		}
	}
}
