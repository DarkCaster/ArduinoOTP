//
// LCGen.cs
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
using System.Security.Cryptography;

namespace OTPManagerApi.Helpers
{
	public sealed class LCGen
	{
		//stuff for powering LCG algorithm with unique 32-bit values (until LGC overflows, of course)
		private ulong curLCGValue;
		//following constants ensures that LGC overflow will occur in 4294967290 rounds (this gives us 2^32-5-1 uinique values)
		//see http://statmath.wu-wien.ac.at/software/prng/doc/prng.html for constant selection details
		private const ulong m = 4294967291UL;
		private const ulong a = 279470273UL;

		public LCGen()
		{
			using (var random = new RNGCryptoServiceProvider())
			{
				//set initial LGC counter to random value
				var bInit = new byte[4];
				random.GetBytes(bInit);
				ulong init = unchecked((uint)(bInit[0] | bInit[1] << 8 | bInit[2] << 16 | bInit[3] << 24));
				if (init < 1)
					init = 1;
				if (init > (m - 1))
					init = m - 1;
				curLCGValue = init;
			}
		}

		public LCGen(uint seed)
		{
			ulong init = seed;
			if (init < 1)
				init = 1;
			if (init > (m - 1))
				init = m - 1;
			curLCGValue = init;
		}

		/// <summary>
		/// Generate next 32-bit pseudo-random value
		/// </summary>
		/// <returns>Next value from LCG</returns>
		public uint GenerateValue()
		{
			curLCGValue = (curLCGValue * a) % m;
			return (uint)curLCGValue;
		}
	}
}
