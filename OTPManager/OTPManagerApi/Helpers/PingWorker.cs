//
// PingWorker.cs
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
using System.Threading;
using System.Threading.Tasks;
using DarkCaster.Async;
using OTPManagerApi.Protocol;

namespace OTPManagerApi.Helpers
{
	public sealed class PingWorker : IDisposable
	{
		public readonly AsyncRWLock pingLock = new AsyncRWLock();
		private readonly Thread pingThread;
		private readonly CancellationTokenSource CTS;
		private readonly Action<Exception> pingThreadFailedCallback;

		public PingWorker(CommHelperBase commHelper, int pingInterval, Action<Exception> pingThreadFailedCallback)
		{
			this.CTS = new CancellationTokenSource();
			this.pingThreadFailedCallback = pingThreadFailedCallback;
			this.pingThread = new Thread(() => Worker(commHelper, CTS.Token, pingInterval));
			this.pingThread.Start();
		}

		private void Worker(CommHelperBase commHelper, CancellationToken token, int pingInterval)
		{
			using (var taskRunner = new AsyncRunner())
			{
				var nullBuff = new byte[0];
				while (!token.IsCancellationRequested)
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
						try { taskRunner.ExecuteTask(commHelper.Resync); }
						catch (Exception ex) { pingThreadFailedCallback?.Invoke(ex); return; }
					}
					finally
					{
						pingLock.ExitWriteLock();
					}
					try { taskRunner.ExecuteTask(() => Task.Delay(pingInterval, token)); }
					catch (TaskCanceledException) { return; }
					catch (Exception ex) { pingThreadFailedCallback?.Invoke(ex); return; }
				}
			}
		}

		public void Dispose()
		{
			CTS.Cancel();
			pingThread.Join();
			CTS.Dispose();
		}
	}
}
