/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
* Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
*
* License terms: Apache 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/**
 * @brief Error handlers
 * @file errors.cpp
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

#include <teseo/utils/errors.h>

// This define can be used when debogging the HAL as a binary
#ifdef STM_TESEO_HAL_ERRORS_TO_IOSTREAM
#include <iostream>
#define ALOGE(message) std::cout << message << std::endl
#else
#define LOG_TAG "teseo_hal_errors"
#include <cutils/log.h>
#endif

#include <errno.h>

int stm::errors::read(int err)
{
	switch(err)
	{
		case EAGAIN:
			ALOGE("read error EAGAIN|EWOULDBLOCK: The file descriptor fd refers to a file other than a socket and has been marked nonblocking (O_NONBLOCK), and the read would block.");
			break;

		case EBADF:
			ALOGE("read error EBADF: fd is not a valid file descriptor or is not open for reading.");
			break;

		case EFAULT:
			ALOGE("read error EFAULT: buf is outside your accessible address space.");
			break;

		case EINTR:
			ALOGE("read error EINTR: The call was interrupted by a signal before any data was read; see signal(7).");
			break;

		case EINVAL:
			ALOGE("read error EINVAL: fd is attached to an object which is unsuitable for reading; or the file was opened with the O_DIRECT flag, and either the address specified in buf, the value specified in count, or the current file offset is not suitably aligned.");
			break;

		case EIO:
			ALOGE("read error EIO: I/O error. This will happen for example when the process is in a background process group, tries to read from its controlling terminal, and either it is ignoring or blocking SIGTTIN or its process group is orphaned. It may also occur when there is a low-level I/O error while reading from a disk or tape.");
			break;

		case EISDIR:
			ALOGE("read error EISDIR: fd refers to a directory.");
			break;

		default:
			ALOGE("read error: Unknown error.");
			break;
	}

	return err;
}

int stm::errors::open(int err)
{
	switch(err)
	{
		case EACCES:
			ALOGE("open error EACCES: The requested access to the file is not allowed, or search permission is denied for one of the directories in the path prefix of pathname, or the file did not exist yet and write access to the parent directory is not allowed. (See also path_resolution(7).)");
			break;
			
		case EDQUOT:
			ALOGE("open error EDQUOT: Where O_CREAT is specified, the file does not exist, and the user's quota of disk blocks or inodes on the filesystem has been exhausted.");
			break;
			
		case EEXIST:
			ALOGE("open error EEXIST: pathname already exists and O_CREAT and O_EXCL were used.");
			break;
			
		case EFAULT:
			ALOGE("open error EFAULT: pathname points outside your accessible address space.");
			break;
			
		case EFBIG:
			ALOGE("open error EFBIG: See EOVERFLOW.");
			break;
			
		case EINTR:
			ALOGE("open error EINTR: While blocked waiting to complete an open of a slow device (e.g., a FIFO; see fifo(7)), the call was interrupted by a signal handler; see signal(7).");
			break;
			
		case EINVAL:
			ALOGE("open error EINVAL: The filesystem does not support the O_DIRECT flag. See NOTES for more information.");
			break;
			
		case EISDIR:
			ALOGE("open error EISDIR: pathname refers to a directory and the access requested involved writing (that is, O_WRONLY or O_RDWR is set).");
			break;
			
		case ELOOP:
			ALOGE("open error ELOOP: Too many symbolic links were encountered in resolving pathname, or O_NOFOLLOW was specified but pathname was a symbolic link.");
			break;
			
		case EMFILE:
			ALOGE("open error EMFILE: The process already has the maximum number of files open.");
			break;
			
		case ENAMETOOLONG:
			ALOGE("open error ENAMETOOLONG: pathname was too long.");
			break;
			
		case ENFILE:
			ALOGE("open error ENFILE: The system limit on the total number of open files has been reached.");
			break;
			
		case ENODEV:
			ALOGE("open error ENODEV: pathname refers to a device special file and no corresponding device exists. (This is a Linux kernel bug; in this situation ENXIO must be returned.)");
			break;
			
		case ENOENT:
			ALOGE("open error ENOENT: O_CREAT is not set and the named file does not exist. Or, a directory component in pathname does not exist or is a dangling symbolic link.");
			break;
			
		case ENOMEM:
			ALOGE("open error ENOMEM: Insufficient kernel memory was available.");
			break;
			
		case ENOSPC:
			ALOGE("open error ENOSPC: pathname was to be created but the device containing pathname has no room for the new file.");
			break;
			
		case ENOTDIR:
			ALOGE("open error ENOTDIR: A component used as a directory in pathname is not, in fact, a directory, or O_DIRECTORY was specified and pathname was not a directory.");
			break;
			
		case ENXIO:
			ALOGE("open error ENXIO: O_NONBLOCK | O_WRONLY is set, the named file is a FIFO and no process has the file open for reading. Or, the file is a device special file and no corresponding device exists.");
			break;
			
		case EOVERFLOW:
			ALOGE("open error EOVERFLOW: pathname refers to a regular file that is too large to be opened. The usual scenario here is that an application compiled on a 32-bit platform without -D_FILE_OFFSET_BITS=64 tried to open a file whose size exceeds (2<<31)-1 bits; see also O_LARGEFILE above. This is the error specified by POSIX.1-2001; in kernels before 2.6.24, Linux gave the error EFBIG for this case.");
			break;
			
		case EPERM:
			ALOGE("open error EPERM: The O_NOATIME flag was specified, but the effective user ID of the caller did not match the owner of the file and the caller was not privileged (CAP_FOWNER).");
			break;
			
		case EROFS:
			ALOGE("open error EROFS: pathname refers to a file on a read-only filesystem and write access was requested.");
			break;
			
		case ETXTBSY:
			ALOGE("open error ETXTBSY: pathname refers to an executable image which is currently being executed and write access was requested.");
			break;
			
		case EWOULDBLOCK:
			ALOGE("open error EWOULDBLOCK: The O_NONBLOCK flag was specified, and an incompatible lease was held on the file (see fcntl(2)).");
			break;

		default:
			ALOGE("open error: Unknown error.");
			break;
	}

	return err;
}

int stm::errors::close(int err)
{
	switch(err)
	{
		case EBADF:
			ALOGE("close error EBADF: fd isn't a valid file descriptor.");
			break;

		case EINTR:
			ALOGE("close error EINTR: the close() call was interrupted by a signal; see signal(7).");
			break;

		case EIO:
			ALOGE("close error EIO: An I/O error occured.");
			break;

		default:
			ALOGE("close error: Unknown error.");
			break;
	}

	return err;
}

int stm::errors::pipe(int err)
{
	switch(err)
	{
		case EFAULT:
			ALOGE("pipe error EFAULT: pipefd is not valid.");
			break;

		case EINVAL:
			ALOGE("pipe error EINVAL: (pipe2()) Invalid value in flags.");
			break;

		case EMFILE:
			ALOGE("pipe error EMFILE: Too many file descriptors are in use by the process.");
			break;

		case ENFILE:
			ALOGE("pipe error ENFILE: The system limit on the total number of open files has been reached.");
			break;

		default:
			ALOGE("pipe error: Unknown error.");
			break;
	}

	return err;
}

int stm::errors::write(int err)
{
	switch(err)
	{
		case EAGAIN:
			ALOGE("write error EAGAIN: The file descriptor fd refers to a file other than a socket and has been marked nonblocking (O_NONBLOCK), and the write would block.\nor\nThe file descriptor fd refers to a socket and has been marked nonblocking (O_NONBLOCK), and the write would block. POSIX.1-2001 allows either error to be returned for this case, and does not require these constants to have the same value, so a portable application should check for both possibilities.");
			break;

		case EBADF:
			ALOGE("write error EBADF: fd is not a valid file descriptor or is not open for writing.");
			break;

		case EDESTADDRREQ:
			ALOGE("write error EDESTADDRREQ: fd refers to a datagram socket for which a peer address has not been set using connect(2).");
			break;

		case EDQUOT:
			ALOGE("write error EDQUOT: The user's quota of disk blocks on the filesystem containing the file referred to by fd has been exhausted.");
			break;

		case EFAULT:
			ALOGE("write error EFAULT: buf is outside your accessible address space.");
			break;

		case EFBIG:
			ALOGE("write error EFBIG: An attempt was made to write a file that exceeds the implementation-defined maximum file size or the process's file size limit, or to write at a position past the maximum allowed offset.");
			break;

		case EINTR:
			ALOGE("write error EINTR: The call was interrupted by a signal before any data was written; see signal(7).");
			break;

		case EINVAL:
			ALOGE("write error EINVAL: fd is attached to an object which is unsuitable for writing; or the file was opened with the O_DIRECT flag, and either the address specified in buf, the value specified in count, or the current file offset is not suitably aligned.");
			break;

		case EIO:
			ALOGE("write error EIO: A low-level I/O error occurred while modifying the inode.");
			break;

		case ENOSPC:
			ALOGE("write error ENOSPC: The device containing the file referred to by fd has no room for the data.");
			break;

		case EPIPE:
			ALOGE("write error EPIPE: fd is connected to a pipe or socket whose reading end is closed. When this happens the writing process will also receive a SIG‐PIPE signal. (Thus, the write return value is seen only if the program catches, blocks or ignores this signal.)");
			break;		

		default:
			ALOGE("write error: Unknown error.");
			break;
	}

	return err;
}