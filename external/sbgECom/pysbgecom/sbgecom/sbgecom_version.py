# -*- coding: utf-8 -*-
#
# ---------------------------------------------------------------------------------------
# *																						* |
# * file				sbgecom_version.py												* |
# * ingroup				src																* |
# * author				SBG Systems														* |
# * date				10 March 2025													* |
# *																						* |
# * brief				Version information.											* |
# * description			python 3.*														* |
# *																						* |
# * copyright			Copyright (C) 2025-2026, SBG Systems SAS. All rights reserved.	* |
# * beginlicense		The MIT license													* |
# *																						* |
# * Permission is hereby granted, free of charge, to any person obtaining a copy		* |
# * of this software and associated documentation files (the "Software"), to deal		* |
# * in the Software without restriction, including without limitation the rights		* |
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell			* |
# * copies of the Software, and to permit persons to whom the Software is				* |
# * furnished to do so, subject to the following conditions:							* |
# *																						* |
# * The above copyright notice and this permission notice shall be included in all		* |
# * copies or substantial portions of the Software.										* |
# *																						* |
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR			* |
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			* |
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE			* |
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER				* |
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,		* |
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE		* |
# * SOFTWARE.																			* |
# *																						* |
# * endlicense																			* |
# *																						* |
# ---------------------------------------------------------------------------------------

class VersionQualifier:
	"""
	Version qualifier definition.
	"""
	DEV						= "dev"
	ALPHA					= "alpha"
	BETA					= "beta"
	RC						= "rc"
	STABLE					= "stable"
	HOT_FIX					= "hotfix"

class SbgEComVersion:

	MAJOR					= 6
	MINOR					= 0
	REV						= 0

	VERSION_BUILD = VersionQualifier.STABLE

	def version_software(major, minor, rev, build):
		return (major << 24) | (minor << 16) | (rev << 8) | (0 if build == VersionQualifier.STABLE else 1)

	VERSION = version_software(
		MAJOR,
		MINOR,
		REV,
		VERSION_BUILD
	)

	def version_string(major, minor, rev, build):
		return f"{major}.{minor}.{rev}-{build}\0"

	VERSION_STR = version_string(
		MAJOR,
		MINOR,
		REV,
		VERSION_BUILD
	)

	def get_version() -> int:
		"""
		Returns an integer representing the version of the sbgECom library.

		Returns:
			int:							An integer representing the version of the sbgECom library.
		"""
		return SbgEComVersion.VERSION

	def get_version_as_string() -> str:
		"""
		Retrieve the sbgECom library version as a string (1.0.443-stable).

		Returns:
			str:							Null terminated string that contains the sbgECom library version.
		"""
		return SbgEComVersion.VERSION_STR
