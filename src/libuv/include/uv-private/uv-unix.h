/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef UV_UNIX_H
#define UV_UNIX_H

#include "ngx-queue.h"

#include "ev.h"
#include "eio.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <termios.h>
#include <pthread.h>

#if __sun
# include <sys/port.h>
# include <port.h>
#endif

/* Note: May be cast to struct iovec. See writev(2). */
typedef struct {
  char* base;
  size_t len;
} uv_buf_t;

typedef int uv_file;

typedef int uv_os_sock_t;

#define UV_ONCE_INIT PTHREAD_ONCE_INIT

typedef pthread_once_t uv_once_t;
typedef pthread_t uv_thread_t;
typedef pthread_mutex_t uv_mutex_t;
typedef pthread_rwlock_t uv_rwlock_t;

/* Platform-specific definitions for uv_spawn support. */
typedef gid_t uv_gid_t;
typedef uid_t uv_uid_t;

/* Platform-specific definitions for uv_dlopen support. */
#define UV_DYNAMIC /* empty */
typedef struct {
  void* handle;
  char* errmsg;
} uv_lib_t;

#define UV_HANDLE_TYPE_PRIVATE /* empty */
#define UV_REQ_TYPE_PRIVATE /* empty */

#if __linux__
# define UV_LOOP_PRIVATE_PLATFORM_FIELDS              \
  /* RB_HEAD(uv__inotify_watchers, uv_fs_event_s) */  \
  struct uv__inotify_watchers {                       \
    struct uv_fs_event_s* rbh_root;                   \
  } inotify_watchers;                                 \
  ev_io inotify_read_watcher;                         \
  int inotify_fd;
#elif defined(PORT_SOURCE_FILE)
# define UV_LOOP_PRIVATE_PLATFORM_FIELDS              \
  ev_io fs_event_watcher;                             \
  int fs_fd;
#else
# define UV_LOOP_PRIVATE_PLATFORM_FIELDS
#endif

#define UV_LOOP_PRIVATE_FIELDS \
  ares_channel channel; \
  /* \
   * While the channel is active this timer is called once per second to be \
   * sure that we're always calling ares_process. See the warning above the \
   * definition of ares_timeout(). \
   */ \
  uv_timer_t timer; \
  /* Poll result queue */ \
  eio_channel uv_eio_channel; \
  struct ev_loop* ev; \
  /* Various thing for libeio. */ \
  uv_async_t uv_eio_want_poll_notifier; \
  uv_async_t uv_eio_done_poll_notifier; \
  uv_idle_t uv_eio_poller; \
  uv_handle_t* pending_handles; \
  ngx_queue_t prepare_handles; \
  ngx_queue_t check_handles; \
  ngx_queue_t idle_handles; \
  UV_LOOP_PRIVATE_PLATFORM_FIELDS

#define UV_REQ_BUFSML_SIZE (4)

#define UV_REQ_PRIVATE_FIELDS  /* empty */

#define UV_WRITE_PRIVATE_FIELDS \
  ngx_queue_t queue; \
  int write_index; \
  uv_buf_t* bufs; \
  int bufcnt; \
  int error; \
  uv_buf_t bufsml[UV_REQ_BUFSML_SIZE];

#define UV_SHUTDOWN_PRIVATE_FIELDS /* empty */

#define UV_CONNECT_PRIVATE_FIELDS \
  ngx_queue_t queue;

#define UV_UDP_SEND_PRIVATE_FIELDS  \
  ngx_queue_t queue;                \
  struct sockaddr_storage addr;     \
  socklen_t addrlen;                \
  uv_buf_t* bufs;                   \
  int bufcnt;                       \
  ssize_t status;                   \
  uv_udp_send_cb send_cb;           \
  uv_buf_t bufsml[UV_REQ_BUFSML_SIZE];  \

#define UV_PRIVATE_REQ_TYPES /* empty */


/* TODO: union or classes please! */
#define UV_HANDLE_PRIVATE_FIELDS \
  int fd; \
  int flags; \
  uv_handle_t* next_pending; \


#define UV_STREAM_PRIVATE_FIELDS \
  uv_connect_t *connect_req; \
  uv_shutdown_t *shutdown_req; \
  ev_io read_watcher; \
  ev_io write_watcher; \
  ngx_queue_t write_queue; \
  ngx_queue_t write_completed_queue; \
  int delayed_error; \
  uv_connection_cb connection_cb; \
  int accepted_fd; \
  int blocking;


/* UV_TCP */
#define UV_TCP_PRIVATE_FIELDS


/* UV_UDP */
#define UV_UDP_PRIVATE_FIELDS         \
  uv_alloc_cb alloc_cb;               \
  uv_udp_recv_cb recv_cb;             \
  ev_io read_watcher;                 \
  ev_io write_watcher;                \
  ngx_queue_t write_queue;            \
  ngx_queue_t write_completed_queue;  \


/* UV_NAMED_PIPE */
#define UV_PIPE_PRIVATE_FIELDS \
  const char* pipe_fname; /* strdup'ed */


/* UV_POLL */
#define UV_POLL_PRIVATE_FIELDS        \
  ev_io io_watcher;


/* UV_PREPARE */ \
#define UV_PREPARE_PRIVATE_FIELDS \
  uv_prepare_cb prepare_cb; \
  ngx_queue_t queue;


/* UV_CHECK */
#define UV_CHECK_PRIVATE_FIELDS \
  uv_check_cb check_cb; \
  ngx_queue_t queue;


/* UV_IDLE */
#define UV_IDLE_PRIVATE_FIELDS \
  ev_idle idle_watcher; \
  uv_idle_cb idle_cb; \
  ngx_queue_t queue;


/* UV_ASYNC */
#define UV_ASYNC_PRIVATE_FIELDS \
  ev_async async_watcher; \
  uv_async_cb async_cb;


/* UV_TIMER */
#define UV_TIMER_PRIVATE_FIELDS \
  ev_timer timer_watcher; \
  uv_timer_cb timer_cb;

#define UV_GETADDRINFO_PRIVATE_FIELDS \
  uv_getaddrinfo_cb cb; \
  struct addrinfo* hints; \
  char* hostname; \
  char* service; \
  struct addrinfo* res; \
  int retcode;

#define UV_PROCESS_PRIVATE_FIELDS \
  ev_child child_watcher;

#define UV_FS_PRIVATE_FIELDS \
  struct stat statbuf; \
  eio_req* eio;

#define UV_WORK_PRIVATE_FIELDS \
  eio_req* eio;

#define UV_TTY_PRIVATE_FIELDS \
  struct termios orig_termios; \
  int mode;

/* UV_FS_EVENT_PRIVATE_FIELDS */
#if defined(__linux__)

#define UV_FS_EVENT_PRIVATE_FIELDS    \
  /* RB_ENTRY(fs_event_s) node; */    \
  struct {                            \
    struct uv_fs_event_s* rbe_left;   \
    struct uv_fs_event_s* rbe_right;  \
    struct uv_fs_event_s* rbe_parent; \
    int rbe_color;                    \
  } node;                             \
  ev_io read_watcher;                 \
  uv_fs_event_cb cb;

#elif defined(__APPLE__)  \
  || defined(__FreeBSD__) \
  || defined(__DragonFly__) \
  || defined(__OpenBSD__) \
  || defined(__NetBSD__)

#define UV_FS_EVENT_PRIVATE_FIELDS \
  ev_io event_watcher; \
  uv_fs_event_cb cb; \
  int fflags; \

#elif defined(__sun)

#ifdef PORT_SOURCE_FILE
# define UV_FS_EVENT_PRIVATE_FIELDS \
  ev_io event_watcher; \
  uv_fs_event_cb cb; \
  file_obj_t fo;
#else /* !PORT_SOURCE_FILE */
# define UV_FS_EVENT_PRIVATE_FIELDS
#endif

#else

/* Stub for platforms where the file watcher isn't implemented yet. */
#define UV_FS_EVENT_PRIVATE_FIELDS

#endif

#endif /* UV_UNIX_H */
