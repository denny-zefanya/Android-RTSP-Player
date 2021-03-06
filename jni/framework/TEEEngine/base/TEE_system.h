#ifndef BASE_SYSTEM_H
#define BASE_SYSTEM_H

#include "TEE_detect.h"

#ifdef __cplusplus
extern "C" {
#endif

void dbg_assert(int test, const char *msg);
#define dbg_assert(test,msg) dbg_assert_imp(__FILE__, __LINE__, test,  msg)
void dbg_assert_imp(const char *filename, int line, int test, const char *msg);

void dbg_break();

void dbg_msg(const char *sys, const char *fmt, ...);

void *mem_alloc_debug(const char *filename, int line, unsigned size, unsigned alignment);
#define mem_alloc(s,a) mem_alloc_debug(__FILE__, __LINE__, (s), (a))

void mem_free(void *block);

void mem_copy(void *dest, const void *source, unsigned size);

void mem_move(void *dest, const void *source, unsigned size);

void mem_zero(void *block, unsigned size);

int mem_comp(const void *a, const void *b, int size);

int mem_check_imp();
#define mem_check() dbg_assert_imp(__FILE__, __LINE__, mem_check_imp(),  "Memory check failed")

enum {
	IOFLAG_READ = 1,
	IOFLAG_WRITE = 2,
	IOFLAG_RANDOM = 4,

	IOSEEK_START = 0,
	IOSEEK_CUR = 1,
	IOSEEK_END = 2
};

typedef struct IOINTERNAL *IOHANDLE;

IOHANDLE io_open(const char *filename, int flags);

unsigned io_read(IOHANDLE io, void *buffer, unsigned size);

unsigned io_skip(IOHANDLE io, int size);

unsigned io_write(IOHANDLE io, const void *buffer, unsigned size);

int io_seek(IOHANDLE io, int offset, int origin);

long int io_tell(IOHANDLE io);

long int io_length(IOHANDLE io);

int io_close(IOHANDLE io);

int io_flush(IOHANDLE io);

IOHANDLE io_stdin();

IOHANDLE io_stdout();

IOHANDLE io_stderr();

void thread_sleep(int milliseconds);

void *thread_create(void (*threadfunc)(void *), void *user);

void thread_wait(void *thread);

void thread_destroy(void *thread);

void thread_yield();

typedef void* LOCK;

LOCK lock_create();
void lock_destroy(LOCK lock);

int lock_try(LOCK lock);
void lock_wait(LOCK lock);
void lock_release(LOCK lock);

#ifdef __GNUC__
__extension__ typedef long long int64;
#else
typedef long long int64;
#endif

int64 time_get();

int64 time_freq();

unsigned time_timestamp();

typedef struct
{
	int type;
	int ipv4sock;
	int ipv6sock;
} NETSOCKET;

enum
{
	NETADDR_MAXSTRSIZE = 1+(8*4+7)+1+1+5+1,
	
	NETTYPE_INVALID = 0,
	NETTYPE_IPV4 = 1,
	NETTYPE_IPV6 = 2,
	NETTYPE_LINK_BROADCAST = 4,
	NETTYPE_ALL = NETTYPE_IPV4|NETTYPE_IPV6
};

typedef struct
{
	unsigned int type;
	unsigned char ip[16];
	unsigned short port;
} NETADDR;

int net_init();

int net_host_lookup(const char *hostname, NETADDR *addr, int types);

int net_addr_comp(const NETADDR *a, const NETADDR *b);

void net_addr_str(const NETADDR *addr, char *string, int max_length);

int net_addr_from_str(NETADDR *addr, const char *string);

NETSOCKET net_udp_create(NETADDR bindaddr);

int net_udp_send(NETSOCKET sock, const NETADDR *addr, const void *data, int size);

int net_udp_recv(NETSOCKET sock, NETADDR *addr, void *data, int maxsize);

int net_udp_close(NETSOCKET sock);

NETSOCKET net_tcp_create(const NETADDR *a);

int net_tcp_listen(NETSOCKET sock, int backlog);

int net_tcp_accept(NETSOCKET sock, NETSOCKET *new_sock, NETADDR *addr);

int net_tcp_connect(NETSOCKET sock, const NETADDR *addr);

int net_tcp_send(NETSOCKET sock, const void *data, int size);

int net_tcp_recv(NETSOCKET sock, void *data, int maxsize);

int net_tcp_close(NETSOCKET sock);

void str_append(char *dst, const char *src, int dst_size);

void strcopy(char *dst, const char *src);

void str_copy(char *dst, const char *src, int dst_size);

int str_length(const char *str);

void str_format(char *buffer, int buffer_size, const char *format, ...);

void str_sanitize_strong(char *str);

void str_sanitize_cc(char *str);

void str_sanitize(char *str);

char *str_skip_to_whitespace(char *str);

char *str_skip_whitespaces(char *str);

int str_comp_nocase(const char *a, const char *b);

int str_comp(const char *a, const char *b);

int str_comp_num(const char *a, const char *b, const int num);

int str_comp_filenames(const char *a, const char *b);

const char *str_find_nocase(const char *haystack, const char *needle);

const char *str_find(const char *haystack, const char *needle);

/*
	Function: str_hex
		Takes a datablock and generates a hexstring of it.

	Parameters:
		dst - Buffer to fill with hex data
		dst_size - size of the buffer
		data - Data to turn into hex
		data - Size of the data

	Remarks:
		- The desination buffer will be zero-terminated
*/
void str_hex(char *dst, int dst_size, const void *data, int data_size);

/*
	Function: str_timestamp
		Copies a time stamp in the format year-month-day_hour-minute-second to the string.

	Parameters:
		buffer - Pointer to a buffer that shall receive the time stamp string.
		buffer_size - Size of the buffer.

	Remarks:
		- Guarantees that buffer string will contain zero-termination.
*/
void str_timestamp(char *buffer, int buffer_size);

/* Group: Filesystem */

/*
	Function: fs_listdir
		Lists the files in a directory
		
	Parameters:
		dir - Directory to list
		cb - Callback function to call for each entry
		type - Type of the directory
		user - Pointer to give to the callback
	
	Returns:
		Always returns 0.
*/
typedef int (*FS_LISTDIR_CALLBACK)(const char *name, int is_dir, int dir_type, void *user);
int fs_listdir(const char *dir, FS_LISTDIR_CALLBACK cb, int type, void *user);

/*
	Function: fs_makedir
		Creates a directory
	
	Parameters:
		path - Directory to create
	
	Returns:
		Returns 0 on success. Negative value on failure.
	
	Remarks:
		Does not create several directories if needed. "a/b/c" will result
		in a failure if b or a does not exist.
*/
int fs_makedir(const char *path);

/*
	Function: fs_storage_path
		Fetches per user configuration directory.
	
	Returns:
		Returns 0 on success. Negative value on failure.
	
	Remarks:
		- Returns ~/.appname on UNIX based systems
		- Returns ~/Library/Applications Support/appname on Mac OS X
		- Returns %APPDATA%/Appname on Windows based systems
*/
int fs_storage_path(const char *appname, char *path, int max);

/*
	Function: fs_is_dir
		Checks if directory exists
	
	Returns:
		Returns 1 on success, 0 on failure.
*/
int fs_is_dir(const char *path);

/*
	Function: fs_chdir
		Changes current working directory
	
	Returns:
		Returns 0 on success, 1 on failure.
*/
int fs_chdir(const char *path);

/*
	Function: fs_getcwd
		Gets the current working directory.
	
	Returns:
		Returns a pointer to the buffer on success, 0 on failure.
*/
char *fs_getcwd(char *buffer, int buffer_size);

/*
	Function: fs_parent_dir
		Get the parent directory of a directory
	
	Parameters:
		path - The directory string

	Returns:
		Returns 0 on success, 1 on failure.

	Remarks:
		- The string is treated as zero-termineted string.
*/
int fs_parent_dir(char *path);

/*
	Function: fs_remove
		Deletes the file with the specified name.
	
	Parameters:
		filename - The file to delete

	Returns:
		Returns 0 on success, 1 on failure.

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
int fs_remove(const char *filename);

/*
	Function: fs_rename
		Renames the file or directory. If the paths differ the file will be moved.
	
	Parameters:
		oldname - The actual name
		newname - The new name

	Returns:
		Returns 0 on success, 1 on failure.

	Remarks:
		- The strings are treated as zero-terminated strings.
*/
int fs_rename(const char *oldname, const char *newname);

/*
	Group: Undocumented
*/


/*
	Function: net_tcp_connect_non_blocking
	
	DOCTODO: serp
*/
int net_tcp_connect_non_blocking(NETSOCKET sock, const NETADDR *a);

/*
	Function: net_tcp_set_non_blocking

	DOCTODO: serp
*/
int net_tcp_set_non_blocking(NETSOCKET sock);

/*
	Function: net_tcp_set_non_blocking

	DOCTODO: serp
*/
int net_tcp_set_blocking(NETSOCKET sock);

/*
	Function: net_errno

	DOCTODO: serp
*/
int net_errno();

/*
	Function: net_would_block

	DOCTODO: serp
*/
int net_would_block();

int net_socket_read_wait(NETSOCKET sock, int time);

void mem_debug_dump(IOHANDLE file);

void swap_endian(void *data, unsigned elem_size, unsigned num);

/* Group: Debug levels */
//by format
enum {
	DBG_FMT_RAW				= 1,	//raw output
	DBG_FMT_TIME			= 2,	//show time
	DBG_FMT_SYS				= 3,	//show sys
	DBG_FMT_FULL			= 4		//show both
};

enum {
	DBG_LEVEL_IMPORTANT			= 0,	//important always showed messages
	DBG_LEVEL_ERROR				= 1,	//error messages
	DBG_LEVEL_WARNING			= 2,	//warning messages
	DBG_LEVEL_MSG				= 3,	//extra debug messages
	DBG_LEVEL_INFO				= 4		//info messages
};

#define DBG_LEVEL_LOW DBG_LEVEL_IMPORTANT
#define DBG_LEVEL_HIGH DBG_LEVEL_INFO

typedef void (*DBG_LOGGER)(const char *line); 
void dbg_logger(DBG_LOGGER logger);

void dbg_logger_stdout();
void dbg_logger_debugger();
void dbg_logger_file(const char *filename);

typedef struct
{
	int allocated;
	int active_allocations;
	int total_allocations;
} MEMSTATS;

const MEMSTATS *mem_stats();

typedef struct
{
	int sent_packets;
	int sent_bytes;
	int recv_packets;
	int recv_bytes;
} NETSTATS;


void net_stats(NETSTATS *stats);

int str_toint(const char *str);
float str_tofloat(const char *str);
int str_isspace(char c);
char str_uppercase(char c);
unsigned str_quickhash(const char *str);

/*
	Function: gui_messagebox
		Display plain OS-dependent message box
	
	Parameters:
		title - title of the message box
		message - text to display
*/
void gui_messagebox(const char *title, const char *message);


/*
	Function: str_utf8_rewind
		Moves a cursor backwards in an utf8 string
	
	Parameters:
		str - utf8 string
		cursor - position in the string

	Returns:
		New cursor position.

	Remarks:
		- Won't move the cursor less then 0
*/
int str_utf8_rewind(const char *str, int cursor);

/*
	Function: str_utf8_forward
		Moves a cursor forwards in an utf8 string
	
	Parameters:
		str - utf8 string
		cursor - position in the string
		
	Returns:
		New cursor position.

	Remarks:
		- Won't move the cursor beyond the zero termination marker
*/
int str_utf8_forward(const char *str, int cursor);

/*
	Function: str_utf8_decode
		Decodes an utf8 character
	
	Parameters:
		ptr - pointer to an utf8 string. this pointer will be moved forward
		
	Returns:
		Unicode value for the character. -1 for invalid characters and 0 for end of string.

	Remarks:
		- This function will also move the pointer forward.
*/
int str_utf8_decode(const char **ptr);

/*
	Function: str_utf8_encode
		Encode an utf8 character
	
	Parameters:
		ptr - Pointer to a buffer that should recive the data. Should be able to hold at least 4 bytes.
		
	Returns:
		Number of bytes put into the buffer.

	Remarks:
		- Does not do zero termination of the string.
*/
int str_utf8_encode(char *ptr, int chr);

/*
	Function: str_utf8_check
		Checks if a strings contains just valid utf8 characters.
	
	Parameters:
		str - Pointer to a possible utf8 string.
		
	Returns:
		0 - invalid characters found.
		1 - only valid characters found.

	Remarks:
		- The string is treated as zero-terminated utf8 string.
*/
int str_utf8_check(const char *str);

#ifdef __cplusplus
}
#endif

#endif
