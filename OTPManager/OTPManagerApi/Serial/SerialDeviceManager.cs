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
	/// <summary>
	/// This class is thread safe
	/// </summary>
	public sealed class SerialDeviceManager : DeviceManagerBase, IOTPDeviceManager
	{
		private StreamCommHelper commHelper;
		private PingWorker pingWorker = null;
		private OTPDeviceState state = OTPDeviceState.Disconnected;

		private readonly SerialProtocolConfig config;
		private readonly SerialPort port;
		private readonly ISafeEventCtrl<OTPDeviceEventArgs> deviceEventCtrl;

		private volatile bool isDisposed = false;

		private SerialDeviceManager(string serialPortName)
		{
			//this init sequence guarantee that no IDisposable objects will be created in case of exception during constructor execution
			config = new SerialProtocolConfig();
			port = new SerialPort(serialPortName, config.SERIAL_PORT_SPEED, Parity.None, 8, StopBits.One);
#if DEBUG
			deviceEventCtrl = new SafeEventDbg<OTPDeviceEventArgs>();
#else
			deviceEventCtrl = new SafeEvent<OTPDeviceEventArgs>();
#endif
		}

		public ISafeEvent<OTPDeviceEventArgs> DeviceEvent => (ISafeEvent<OTPDeviceEventArgs>)deviceEventCtrl;

		protected override CommHelperBase CommHelper => commHelper;

		public async Task Connect()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			if (state != OTPDeviceState.Disconnected)
				throw new Exception("Device manager is in wrong state: " + state.ToString());
			try
			{
				port.Open();
				commHelper = new StreamCommHelper(port.BaseStream, config);
				await BaseConnect(); //run base connection routine
			}
			catch (Exception)
			{
				try { commHelper = null; } //TODO: state change
				catch (Exception) { }
				throw;
			}
			//run ping worker thread
			pingWorker = new PingWorker(CommHelper, config.PING_INTERVAL, null); //TODO: fail callback
			//TODO: state change
		}

		public Task Disconnect()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			try
			{
				pingWorker.Dispose(); //kill ping worker thread (should not throw exceptions)
				port.Close();
			}
			catch (Exception)
			{
				//TODO state change, send notification
				throw;
			}
			finally
			{
				pingWorker = null;
				commHelper = null;
			}
			//TODO state change, send notification
			return Task.FromResult(true);
		}

		public void Dispose()
		{
			if (isDisposed)
				return;
			isDisposed = true;
			//TODO: call disconnect
			pingWorker?.Dispose();
			port.Dispose();
			deviceEventCtrl.Dispose();
		}
	}
}
