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
using DarkCaster.Async;
using OTPManagerApi.Helpers;
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Serial
{
	/// <summary>
	/// This class is thread safe
	/// </summary>
	public sealed class SerialDeviceManager : DeviceManagerBase, IOTPDeviceManager
	{
		private PingWorker pingWorker = null;
		private OTPDeviceState state = OTPDeviceState.Disconnected;

		private readonly AsyncRunner asyncRunner = new AsyncRunner();
		private readonly SerialProtocolConfig config;
		private readonly SerialPort port;
		private readonly ISafeEventCtrlLite<OTPDeviceEventArgs> deviceEventCtrl;

		//fields shared between ping-worker and main thread:
		private readonly AsyncRWLock pingLock = new AsyncRWLock();
		//protected readonly LCGen reqLCG = new LCGen(0); // defined at DeviceManagerBase
		//protected readonly LCGen ansLCG = new LCGen(0); // defined at DeviceManagerBase
		private StreamCommHelper commHelper; //created on connect

		private volatile bool isDisposed = false;

		public SerialDeviceManager(string serialPortName)
		{
			//this init sequence guarantee that no IDisposable objects will be created in case of exception during constructor execution
			config = new SerialProtocolConfig();
			port = new SerialPort(serialPortName, config.SERIAL_PORT_SPEED, Parity.None, 8, StopBits.One)
			{
				ReadTimeout = config.CMD_TIMEOUT,
				WriteTimeout = config.CMD_TIMEOUT
			};
#if DEBUG
			deviceEventCtrl = new SafeEventDbg<OTPDeviceEventArgs>();
#else
			deviceEventCtrl = new SafeEvent<OTPDeviceEventArgs>();
#endif
		}

		public ISafeEvent<OTPDeviceEventArgs> DeviceEvent => (ISafeEvent<OTPDeviceEventArgs>)deviceEventCtrl;

		protected override CommHelperBase CommHelper => commHelper;

		protected override ProtocolConfig Config => config;

		protected override void PreCommandCommit()
		{
			port.WriteTimeout = config.COMMIT_TIMEOUT;
			port.ReadTimeout = config.COMMIT_TIMEOUT;
			commHelper.UseCommitTimeout = true;
		}

		protected override void PostCommandCommit()
		{
			port.WriteTimeout = config.CMD_TIMEOUT;
			port.ReadTimeout = config.CMD_TIMEOUT;
			commHelper.UseCommitTimeout = false;
		}

		private class PingFailedCallbackException : Exception { }

		public void PingFailedCallback(Exception ex)
		{
			try
			{
				deviceEventCtrl.Raise(this, () =>
				{
					if (state == OTPDeviceState.Connected)
					{
						state = OTPDeviceState.Failed;
						return new OTPDeviceEventArgs() { State = OTPDeviceState.Failed, Error = ex };
					}
					throw new PingFailedCallbackException();
				});
			}
			catch (PingFailedCallbackException) { }
		}

		public async Task ConnectAsync()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			Exception connectException = null;
			await deviceEventCtrl.RaiseAsync(this,async () =>
			{
				if (state != OTPDeviceState.Disconnected)
					throw new Exception("Device manager is in wrong state: " + state.ToString());
				try
				{
					port.Open();
					commHelper = new StreamCommHelper(port.BaseStream, config);
					var lcgInitValues = await CommHelper.Resync(); //run resync;
					reqLCG.ReInit(lcgInitValues.Key);
					ansLCG.ReInit(lcgInitValues.Value);
				}
				catch (Exception ex)
				{
					commHelper = null;
					connectException = ex;
					state = OTPDeviceState.Failed;
					return new OTPDeviceEventArgs() { State = OTPDeviceState.Failed, Error = ex };
				}
				//run ping worker thread
				pingWorker = PingWorker.StartNewWorker(pingLock, CommHelper, reqLCG, ansLCG, config.PING_INTERVAL, PingFailedCallback);
				//state change
				state = OTPDeviceState.Connected;
				return new OTPDeviceEventArgs() { State = OTPDeviceState.Connected, Error = null };
			});
			if (connectException != null)
				throw connectException;
		}

		public async Task DisconnectAsync()
		{
			if (isDisposed)
				throw new ObjectDisposedException("SerialDeviceManager");
			Exception disconnectException = null;
			await deviceEventCtrl.RaiseAsync(this, () =>
			{
				if (state != OTPDeviceState.Connected)
					throw new Exception("Device manager is in wrong state: " + state.ToString());
				try
				{
					pingWorker.Stop(); //kill ping worker thread (should not throw exceptions)
					port.Close();
				}
				catch (Exception ex)
				{
					disconnectException = ex;
					state = OTPDeviceState.Failed;
					return Task.FromResult(new OTPDeviceEventArgs() { State = OTPDeviceState.Failed, Error = ex });
				}
				finally
				{
					pingWorker = null;
					commHelper = null;
				}
				//state change
				state = OTPDeviceState.Disconnected;
				return Task.FromResult(new OTPDeviceEventArgs() { State = OTPDeviceState.Disconnected, Error = null });
			});
			if (disconnectException != null)
				throw disconnectException;
		}

		public void Connect()
		{
			asyncRunner.ExecuteTask(ConnectAsync);
		}

		public void Disconnect()
		{
			asyncRunner.ExecuteTask(DisconnectAsync);
		}

		public void Dispose()
		{
			if (isDisposed)
				return;
			try { asyncRunner.ExecuteTask(DisconnectAsync); }
			catch (Exception) { };
			isDisposed = true;
			pingWorker?.Stop();
			port.Dispose();
		}
	}
}
