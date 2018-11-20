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
	public sealed class PingWorker
	{
		private readonly Thread pingThread;
		private readonly CancellationTokenSource CTS;

		private PingWorker() { }

		private PingWorker(AsyncRWLock commLock, CommHelperBase sharedCommHelper, LCGen sharedReqLCG, LCGen sharedAnsLCG, int pingInterval, Action<Exception> pingThreadFailedCallback)
		{
			this.CTS = new CancellationTokenSource();
			this.pingThread = new Thread(() => Worker(commLock, sharedCommHelper, sharedReqLCG, sharedAnsLCG, pingInterval, CTS.Token, pingThreadFailedCallback));
		}

		public static PingWorker StartNewWorker(AsyncRWLock commLock, CommHelperBase sharedCommHelper, LCGen sharedReqLCG, LCGen sharedAnsLCG, int pingInterval, Action<Exception> pingThreadFailedCallback)
		{
			var result = new PingWorker(commLock, sharedCommHelper, sharedReqLCG, sharedAnsLCG, pingInterval, pingThreadFailedCallback);
			result.pingThread.Start();
			return result;
		}

		private void Worker(AsyncRWLock commLock, CommHelperBase commHelper, LCGen reqLCG, LCGen ansLCG, int pingInterval, CancellationToken token, Action<Exception> pingThreadFailedCallback)
		{
			using (var taskRunner = new AsyncRunner())
			{
				var nullBuff = new byte[0];
				while (!token.IsCancellationRequested)
				{
					commLock.EnterWriteLock();
					try
					{
						Answer answer = Answer.Invalid;
						taskRunner.AddTask(() => commHelper.SendRequest(ReqType.Ping, reqLCG.GenerateValue(), nullBuff, 0, 0));
						taskRunner.AddTask(commHelper.ReceiveAnswer, (obj) => answer = obj);
						taskRunner.RunPendingTasks();
						if (answer.ansType != AnsType.Pong || answer.seq!=ansLCG.GenerateValue())
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
						commLock.ExitWriteLock();
					}
					try { taskRunner.ExecuteTask(() => Task.Delay(pingInterval, token)); }
					catch (TaskCanceledException) { return; }
					catch (Exception ex) { pingThreadFailedCallback?.Invoke(ex); return; }
				}
			}
		}

		public void Stop()
		{
			CTS.Cancel();
			pingThread.Join();
			CTS.Dispose();
		}
	}
}
