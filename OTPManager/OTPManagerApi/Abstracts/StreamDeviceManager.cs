//
// StreamDeviceManager.cs
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
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using DarkCaster.Async;
using DarkCaster.Events;
using OTPManagerApi.Helpers;
using OTPManagerApi.Protocol;

namespace OTPManagerApi
{
	public abstract class StreamDeviceManager : IOTPDeviceManager
	{
		private readonly AsyncRWLock pingLock = new AsyncRWLock();
		private volatile Exception pingThreadException = null;
		private volatile Thread pingThread = null;
		private readonly CancellationToken pingThreadCT;

		protected readonly CancellationTokenSource pingThreadCTS;
		protected readonly ICommHelper commHelper;
		protected readonly ResyncHelper resyncHelper;
		protected readonly Stream link;
		protected readonly ISafeEventCtrl<OTPDeviceEventArgs> deviceEventCtl;
		protected readonly ProtocolConfig config;

		public abstract ISafeEvent<OTPDeviceEventArgs> DeviceEvent { get; }

		protected StreamDeviceManager(Stream link, ISafeEventCtrl<OTPDeviceEventArgs> deviceEventCtl, ProtocolConfig config)
		{
			this.link = link;
			this.deviceEventCtl = deviceEventCtl;
			this.commHelper = new StreamCommHelper(link, config);
			this.resyncHelper = new ResyncHelper(commHelper, config);
			this.config = config;
			this.pingThreadCTS = new CancellationTokenSource();
			this.pingThreadCT = pingThreadCTS.Token;
		}

		private void PingThread()
		{
			var taskRunner = new AsyncRunner();
			var nullBuff = new byte[0];
			while (!pingThreadCT.IsCancellationRequested)
			{
				pingLock.EnterWriteLock();
				try
				{
					Answer answer = Answer.Invalid;
					taskRunner.AddTask(() => commHelper.SendRequest(ReqType.Ping, nullBuff, 0, 0));
					taskRunner.AddTask(commHelper.ReceiveAnswer, (obj) => answer = obj);
					taskRunner.RunPendingTasks();
					if (answer.ansType != AnsType.Pong)
						throw new Exception();
				}
				catch (Exception)
				{
					//try to resync
					try { taskRunner.ExecuteTask(resyncHelper.Resync); }
					//TODO: send event and set apropriate state
					catch (Exception ex) { pingThreadException = ex; return; }
				}
				finally
				{
					pingLock.ExitWriteLock();
				}
				try { taskRunner.ExecuteTask(() => Task.Delay(config.PING_INTERVAL, pingThreadCT)); }
				catch (TaskCanceledException) { return; }
				//TODO: send event and set apropriate state
				catch (Exception ex) { pingThreadException = ex; return; }
			}
		}

		public virtual async Task Connect()
		{
			//run resync
			await resyncHelper.Resync();
			//run ping thread
			pingThread = new Thread(PingThread);
			pingThread.Start();
		}

		public virtual async Task Disconnect()
		{
			pingThreadCTS.Cancel();
			await pingLock.EnterReadLockAsync();
			pingLock.ExitReadLock();
			pingThread?.Join();
			pingThread = null;
			//TODO: send event and set apropriate state
		}

		public void Dispose()
		{
			if (pingThread != null)
			{
				try
				{
					var taskRunner = new AsyncRunner();
					taskRunner.ExecuteTask(Disconnect);
				}
				catch (Exception ex) { pingThreadException = ex; }
			}
			deviceEventCtl.Dispose();
		}
	}
}
