/*
 *
 * ©K. D. Hedger. Sun  5 Feb 18:28:11 GMT 2017 kdhedger68713@gmail.com

 * This file (mimetype.cpp) is part of KKFileManager.

 * KKFileManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKFileManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKFileManager.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
Large portions of code in this file are from the great filemanager SpaceFM, available here:
http://ignorantguru.github.io/spacefm/
HUGE thanks to ignorantguru for his great work.
*/

#include <fcntl.h>
#include <sys/mman.h>
#include <fnmatch.h>

#include "globals.h"

/* cache header */
#define MAJOR_VERSION 0
#define MINOR_VERSION 2
#define ALIAS_LIST 4
#define PARENT_LIST 8
#define LITERAL_LIST 12
#define SUFFIX_TREE 16
#define GLOB_LIST 20
#define MAGIC_LIST 24
#define LIB_MAX_MINOR_VERSION 2
#define LIB_MIN_MINOR_VERSION 0
#define LIB_MAJOR_VERSION 1

#define VAL16(buffrer,idx) GUINT16_FROM_BE(*(guint16*)(buffer+idx))
#define VAL32(buffer,idx) GUINT32_FROM_BE(*(guint32*)(buffer+idx))

#define XDG_MIME_TYPE_UNKNOWN "application/octet-stream"
#define XDG_MIME_TYPE_DIRECTORY "inode/directory"
#define XDG_MIME_TYPE_EXECUTABLE "application/x-executable"
#define XDG_MIME_TYPE_PLAIN_TEXT "text/plain"
#define TEXT_MAX_EXTENT 512

struct MimeCache
{
	char		*file_path;
	gboolean	has_reverse_suffix : 1; /* since mime.cache v1.1,shared mime info v0.4 */
	gboolean	has_str_weight : 1; /* since mime.cache v1.1,shared mime info v0.4 */
	const char	*buffer;
	guint size;

	guint32		n_alias;
	const char	*alias;

	guint32		n_parents;
	const char	*parents;

	guint32		n_literals;
	const char	*literals;

	guint32		n_globs;
	const char	*globs;

	guint32		n_suffix_roots;
	const char	*suffix_roots;

	guint32		n_magics;
	guint32		magic_max_extent;
	const char	*magics;
};

MimeCache		**caches=NULL;
guint			n_caches=0;
guint32			mime_cache_max_extent=0;
char			*mime_magic_buf=NULL;

gboolean mime_type_is_data_plain_text(const char* data,int len)
{
	if(len>=0 && data)
		{
			for(int i=0; i<len; ++i)
				{
					if(data[i]=='\0')
						return(false);
				}
			return(true);
		}
	return(false);
}

static gboolean magic_rule_match(const char* buf,const char* rule,const char* data,unsigned len)
{
	gboolean	match=false;
	guint32		offset=VAL32(rule,0);
	guint32		range=VAL32(rule,4);

	guint32		max_offset=offset+range;
	guint32		val_len=VAL32(rule,12);

	for(offset=VAL32(rule,0);offset<max_offset && (offset+val_len) <= len;++offset)
		{
			guint32	val_off=VAL32(rule,16);
			guint32	mask_off=VAL32(rule,20);
			const char	*value=buf+val_off;

			if(mask_off>0)    /* compare with mask applied */
				{
					unsigned	i=0;
					const char	*mask=buf+mask_off;

					for(i=0; i<val_len; ++i)
						{
							if((data[offset+i] & mask[i])!=value[i])
								break;
						}
					if(i>=val_len)
						match=true;
				}
			else
				{
					if(memcmp(value,data+offset,val_len)==0)
						match=true;
				}

			if(match)
				{
					guint32	n_children=VAL32(rule,24);
					if(n_children>0)
						{
							guint32	first_child_off=VAL32(rule,28);
							guint i;
							rule=buf+first_child_off;
							for(i=0; i<n_children; ++i,rule+=32)
								{
									if(magic_rule_match(buf,rule,data,len))
										return(true);
								}
						}
					else
						return(true);
				}
		}
	return(false);
}

static gboolean magic_match(const char* buf,const char* magic,const char* data,int len)
{
	guint32		n_rules=VAL32(magic,8);
	guint32		rules_off=VAL32(magic,12);
	const char	*rule=buf+rules_off;

	for(unsigned i=0; i<n_rules; ++i,rule+=32)
		if(magic_rule_match(buf,rule,data,len))
			return(true);
	return(false);
}

static void mime_cache_unload(MimeCache* cache,gboolean clear)
{
	if(cache->buffer)
		munmap((char*)cache->buffer,cache->size);

	g_free((gpointer)cache->file_path);
	if(clear==true)
		memset(cache,0,sizeof(MimeCache));
}

const char* mime_cache_lookup_magic(MimeCache* cache,const char* data,int len)
{
	const char	*magic=cache->magics;

	if(! data || (0==len) || ! magic)
		return(NULL);

	for(unsigned i=0; i<cache->n_magics;++i,magic+=16)
		{
			if(magic_match(cache->buffer,magic,data,len))
				return(cache->buffer+VAL32(magic,4));
		}
	return(NULL);
}

gboolean mime_cache_load(MimeCache* cache,const char* file_path)
{
	guint		majv,minv;
	int			fd=-1;
	struct stat	statbuf;
	char		*buffer=NULL;
	guint32		offset;

	/* Unload old cache first if needed */
	if(file_path==cache->file_path)
		cache->file_path=NULL; /* steal the string to prevent it from being freed during unload */
	mime_cache_unload(cache,true);

	/* Store the file path */
	cache->file_path=g_strdup(file_path);

	/* Open the file and map it into memory */
	fd=open (file_path,O_RDONLY,0);

	if(fd<0)
		return(false);

	if(fstat (fd,&statbuf)<0)
		{
			close(fd);
			return(false);
		}

	buffer=(char*)mmap(NULL,statbuf.st_size,PROT_READ,MAP_SHARED,fd,0);
	close(fd);

	if(buffer==(void*)-1)
		return(false);

	majv=VAL16(buffer,MAJOR_VERSION);
	minv=VAL16(buffer,MINOR_VERSION);

	/* Check version */
	if(majv>LIB_MAJOR_VERSION || minv>LIB_MAX_MINOR_VERSION || minv<LIB_MIN_MINOR_VERSION)
		{
			munmap (buffer,statbuf.st_size);
			return(false);
		}

	/* Since mime.cache v1.1,shared mime info v0.4
	 * suffix tree is replaced with reverse suffix tree,
	 * and glob and literal strings are sorted by weight. */
	if(minv>=1)
		{
			cache->has_reverse_suffix=true;
			cache->has_str_weight=true;
		}

	cache->buffer=buffer;
	cache->size=statbuf.st_size;

	offset=VAL32(buffer,ALIAS_LIST);
	cache->alias=buffer+offset+4;
	cache->n_alias=VAL32(buffer,offset);

	offset=VAL32(buffer,PARENT_LIST);
	cache->parents=buffer+offset+4;
	cache->n_parents=VAL32(buffer,offset);

	offset=VAL32(buffer,LITERAL_LIST);
	cache->literals=buffer+offset+4;
	cache->n_literals=VAL32(buffer,offset);

	offset=VAL32(buffer,GLOB_LIST);
	cache->globs=buffer+offset+4;
	cache->n_globs=VAL32(buffer,offset);

	offset=VAL32(buffer,SUFFIX_TREE);
	cache->suffix_roots=buffer+VAL32(buffer+offset,4);
	cache->n_suffix_roots=VAL32(buffer,offset);

	offset=VAL32(buffer,MAGIC_LIST);
	cache->n_magics=VAL32(buffer,offset);
	cache->magic_max_extent=VAL32(buffer+offset,4);
	cache->magics=buffer+VAL32(buffer+offset,8);

	return(true);
}

MimeCache* mime_cache_new(const char* file_path)
{
	MimeCache	*cache=NULL;

	cache=g_new0(MimeCache,1);
	if(file_path)
		mime_cache_load(cache,file_path);
	return(cache);
}

void mime_cache_load_all()
{
	const char	*const *dirs;
	const char filename[]="/mime/mime.cache";
	char* path;

	dirs=g_get_system_data_dirs();
	n_caches=g_strv_length((char**)dirs)+1;
	caches=(MimeCache**)malloc(n_caches * sizeof(MimeCache*));

	path=g_build_filename(g_get_user_data_dir(),filename,NULL);

	caches[0]=mime_cache_new(path);
	g_free(path);
	if(caches[0]->magic_max_extent>mime_cache_max_extent)
		mime_cache_max_extent=caches[0]->magic_max_extent;

	for(unsigned i=1; i<n_caches; ++i)
		{
			path=g_build_filename(dirs[i - 1],filename,NULL);
			caches[i]=mime_cache_new(path);
			g_free(path);
			if(caches[i]->magic_max_extent>mime_cache_max_extent)
				mime_cache_max_extent=caches[i]->magic_max_extent;
		}
	mime_magic_buf=(char*)malloc(mime_cache_max_extent);
	return;
}

void mime_type_init()
{
	mime_cache_load_all();
}

static const char* lookup_suffix_nodes(const char* buf,const char* nodes,guint32 n,const char* name)
{
	gunichar	uchar;
	int			upper=n;
	int			lower=0;
	int			middle=n/2;

	uchar=g_unichar_tolower(g_utf8_get_char(name));

	while(upper>=lower)
		{
			const char	*node=nodes+middle*16;
			guint32		ch=VAL32(node,0);

			if(uchar<ch)
				upper=middle - 1;
			else if(uchar>ch)
				lower=middle+1;
			else /* uchar==ch */
				{
					guint32	n_children=VAL32(node,8);
					name=g_utf8_next_char(name);

					if(n_children>0)
						{
							guint32	first_child_off;
							if(uchar==0)
								return(NULL);

							if(! name || 0==name[0])
								{
									guint32	offset=VAL32(node,4);
									return(offset ? buf+offset : NULL);
								}
							first_child_off=VAL32(node,12);
							return(lookup_suffix_nodes(buf,(buf+first_child_off),n_children,name));
						}
					else
						{
							if(! name || 0==name[0])
								{
									guint32	offset=VAL32(node,4);
									return(offset ? buf+offset : NULL);
								}
							return(NULL);
						}
				}
			middle=(upper+lower)/2;
		}
	return(NULL);
}

/* Reverse suffix tree is used since mime.cache 1.1 (shared mime info 0.4)
 * Returns the address of the found "node",not mime-type.
 * FIXME: 1. Should be optimized with binary search
 *        2. Should consider weight of suffix nodes
 */
static const char* lookup_reverse_suffix_nodes(const char* buf,const char* nodes,guint32 n,const char* name,const char* suffix,const char** suffix_pos)
{
	const char	*ret=NULL;
	const char	*_suffix_pos=NULL;
	const char	*cur_suffix_pos=(const char*)suffix+1;
	const char	*leaf_node=NULL;
	gunichar	uchar;

	uchar=suffix ? g_unichar_tolower(g_utf8_get_char(suffix)) : 0;

	for(unsigned i=0; i<n; ++i)
		{
			const char	*node =nodes+i * 12;
			guint32	ch=VAL32(node,0);
			_suffix_pos=suffix;
			if(ch)
				{
					if(ch==uchar)
						{
							guint32	n_children=VAL32(node,4);
							guint32	first_child_off=VAL32(node,8);
							leaf_node=lookup_reverse_suffix_nodes(buf,buf+first_child_off,n_children,name,g_utf8_find_prev_char(name,suffix),&_suffix_pos);
							if(leaf_node && _suffix_pos<cur_suffix_pos)
								{
									ret=leaf_node;
									cur_suffix_pos=_suffix_pos;
								}
						}
				}
			else /* ch==0 */
				{
					if(suffix<cur_suffix_pos)
						{
							ret=node;
							cur_suffix_pos=suffix;
						}
				}
		}
	*suffix_pos=cur_suffix_pos;
	return(ret);
}

const char* mime_cache_lookup_suffix(MimeCache* cache,const char* filename,const char** suffix_pos)
{
	const char	*root=cache->suffix_roots;
	int			n=cache->n_suffix_roots;
	const char	*mime_type=NULL,*ret=NULL,*prev_suffix_pos=(const char*)-1;
	int			fn_len;

	if(! filename || ! *filename || 0==n)
		return(NULL);
	if(cache->has_reverse_suffix)  /* since mime.cache ver: 1.1 */
		{
			const char *suffix;
			const char *leaf_node;
			const char *_suffix_pos=(const char*)-1;
			fn_len=strlen(filename);
			suffix=g_utf8_find_prev_char(filename,filename+fn_len);
			leaf_node=lookup_reverse_suffix_nodes(cache->buffer,root,n,filename,suffix,&_suffix_pos);
			if(leaf_node)
				{
					mime_type=cache->buffer+VAL32(leaf_node,4);
					*suffix_pos=_suffix_pos;
					ret=mime_type;
				}
		}
	else  /* before mime.cache ver: 1.1 */
		{
			for(int i=0; i <n; ++i,root+=16)
				{
					guint32		first_child_off;
					guint32		ch=VAL32(root,0);
					const char	*suffix;

					suffix=strchr(filename,ch);
					if(! suffix)
						continue;

					first_child_off=VAL32(root,12);
					n=VAL32(root,8);
					do
						{
							mime_type=lookup_suffix_nodes(cache->buffer,cache->buffer+first_child_off,n,g_utf8_next_char(suffix));
							if(mime_type && suffix<prev_suffix_pos) /* we want the longest suffix matched. */
								{
									ret=mime_type;
									prev_suffix_pos=suffix;
								}
						}
					while((suffix=strchr(suffix+1,ch)));
				}
			*suffix_pos=ret ? prev_suffix_pos : (const char*)-1;
		}
	return(ret);
}

static const char* lookup_str_in_entries(MimeCache* cache,const char* entries,int n,const char* str)
{
	int	upper=n;
	int	lower=0;
	int	middle=upper/2;

	if(entries && str && *str)
		{
			/* binary search */
			while(upper>=lower)
				{
					const char	*entry=entries+middle * 8;
					const char	*str2=cache->buffer+VAL32(entry,0);
					int	comp=strcmp(str,str2);
					if(comp<0)
						upper=middle - 1;
					else if(comp>0)
						lower=middle+1;
					else /* comp==0 */
						return((cache->buffer+ VAL32(entry,4)));
					middle=(upper+lower) / 2;
				}
		}
	return(NULL);
}

const char* mime_cache_lookup_literal(MimeCache* cache,const char* filename)
{
	if(cache->has_str_weight)
		{
			const char	*entries=cache->literals;
			int			n=cache->n_literals;
			int			upper=n;
			int			lower=0;
			int			middle=upper/2;

			if(entries && filename && *filename)
				{
					while(upper>=lower)
						{
							const char	*entry=entries+middle * 12;
							const char	*str2=cache->buffer+VAL32(entry,0);
							int	comp=strcmp(filename,str2);
							if(comp<0)
								upper=middle - 1;
							else if(comp>0)
								lower=middle+1;
							else /* comp==0 */
								return((cache->buffer+ VAL32(entry,4)));
							middle=(upper+lower) / 2;
						}
				}
			return(NULL);
		}
	return(lookup_str_in_entries(cache,cache->literals,cache->n_literals,filename));
}

const char* mime_cache_lookup_glob(MimeCache* cache,const char* filename,int *glob_len)
{
	const char	*entry=cache->globs,*type=NULL;
	int			max_glob_len=0;

	/* entry size is changed in mime.cache 1.1 */
	size_t entry_size=cache->has_str_weight ? 12 : 8;

	for(unsigned i=0; i<cache->n_globs; ++i)
		{
			const char	*glob=cache->buffer+VAL32(entry,0);
			int	_glob_len;
			if(fnmatch(glob,filename,0)==0 && (_glob_len=strlen(glob))>max_glob_len)
				{
					max_glob_len=_glob_len;
					type=(cache->buffer+VAL32(entry,4));
				}
			entry+=entry_size;
		}
	*glob_len=max_glob_len;
	return(type);
}

const char* mime_type_get_by_filename(const char* filename,struct stat* statbuf)
{
	const char	*type=NULL,*suffix_pos=NULL,*prev_suffix_pos=(const char*)-1;
	MimeCache	*cache;

	if(statbuf && S_ISDIR(statbuf->st_mode))
		return(XDG_MIME_TYPE_DIRECTORY);

	for(unsigned i=0; ! type && i<n_caches; ++i)
		{
			cache=caches[i];
			type=mime_cache_lookup_literal(cache,filename);
			if(! type)
				{
					const char	*_type=mime_cache_lookup_suffix(cache,filename,&suffix_pos);
					if(_type && suffix_pos<prev_suffix_pos)
						{
							type=_type;
							prev_suffix_pos=suffix_pos;
						}
				}
		}

	if(! type)  /* glob matching */
		{
			int	max_glob_len=0,glob_len=0;
			for(unsigned i=0; ! type && i<n_caches; ++i)
				{
					cache=caches[i];
					const char	*matched_type;
					matched_type=mime_cache_lookup_glob(cache,filename,&glob_len);
					/* according to the mime.cache 1.0 spec,we should use the longest glob matched. */
					if(matched_type && glob_len>max_glob_len)
						{
							type=matched_type;
							max_glob_len=glob_len;
						}
				}
		}

	return(type && *type ? type : XDG_MIME_TYPE_UNKNOWN);
}

const char* mime_type_get_by_file(const char* filepath,struct stat* statbuf,const char* basename)
{
	const char	*type;
	struct stat _statbuf;

	if(statbuf==NULL || S_ISLNK(statbuf->st_mode))
		{
			statbuf=&_statbuf;
			if(stat (filepath,statbuf)==-1)
				return(XDG_MIME_TYPE_UNKNOWN);
		}

	if(S_ISDIR(statbuf->st_mode))
		return(XDG_MIME_TYPE_DIRECTORY);

	if(basename==NULL)
		{
			basename=g_utf8_strrchr(filepath,-1,'/');
			if(basename)
				++basename;
			else
				basename=filepath;
		}

	if(basename)
		{
			type=mime_type_get_by_filename(basename,statbuf);
			if(strcmp(type,XDG_MIME_TYPE_UNKNOWN))
				return(type);
			type=NULL;
		}

	if(statbuf->st_size>0 && !S_ISFIFO(statbuf->st_mode))
		{
			int	fd=-1;
			char* data;

			/* Open the file and map it into memory */
			fd=open (filepath,O_RDONLY,0);
			if(fd!=-1)
				{
					int	len=mime_cache_max_extent;
					if(mime_cache_max_extent>statbuf->st_size)
						len=statbuf->st_size;

					data=(char*) mmap(NULL,len,PROT_READ,MAP_SHARED,fd,0);

					if(data!=(void*)-1)
						{
							for(unsigned i=0; ! type && i<n_caches; ++i)
								type=mime_cache_lookup_magic(caches[i],data,len);

							/* Check for executable file */
							if(! type && g_file_test(filepath,G_FILE_TEST_IS_EXECUTABLE))
								type=XDG_MIME_TYPE_EXECUTABLE;

							/* fallback: check for plain text */
							if(! type)
								{
									if(mime_type_is_data_plain_text(data,len>TEXT_MAX_EXTENT ? TEXT_MAX_EXTENT : len))
										type=XDG_MIME_TYPE_PLAIN_TEXT;
								}

							munmap ((char*)data,len);
						}
					close(fd);
				}
		}
	else
		{
			/* empty file can be viewed as text file */
			type=XDG_MIME_TYPE_PLAIN_TEXT;
		}
	return(type && *type ? type : XDG_MIME_TYPE_UNKNOWN);
}

char *getDIcon(const char *mime_type,const char *name)
{
	char			*file;

	GtkIconInfo* inf=gtk_icon_theme_lookup_icon(defaultTheme,name,48,GTK_ICON_LOOKUP_GENERIC_FALLBACK);

	if( ! inf )
		return(NULL);
	file=strdup(gtk_icon_info_get_filename(inf));
	gtk_icon_info_free(inf);

	return(file);

}

char *getFIcon(const char *mime_type,char *name)
{
	const char		*sep;
	char			icon_name[100];
	char			*file;
	char			*file1;

//printf(">>>%s<<<\n", mime_type);
	sep=strchr( mime_type, '/' );
	if ( sep )
		{
			/* convert mime-type foo/bar to foo-bar */
			strcpy( icon_name, mime_type );
			icon_name[(sep - mime_type)]='-';
			/* is there an icon named foo-bar? */
			file=getDIcon ( NULL, icon_name );
			if ( ! file )
				{
					/* maybe we can find a legacy icon named gnome-mime-foo-bar */
					strcpy( icon_name, "gnome-mime-" );
					strncat( icon_name, mime_type, ( sep - mime_type ));
					strcat( icon_name, "-" );
					strcat( icon_name, sep + 1 );
					file=getDIcon ( NULL, icon_name );
					//icon=vfs_load_icon ( icon_theme, icon_name, size );
				}
//        // hack for x-xz-compressed-tar missing icon
//printf(">>>%s-%s<<<\n", strstr( mime_type, "compressed" ), mime_type);
//        if ( !icon && strstr( mime_type, "compressed" ))
//        {
//            icon=vfs_load_icon ( icon_theme, "application-x-archive", size );
//            if ( !icon )
//                icon=vfs_load_icon ( icon_theme, "gnome-mime-application-x-archive",
//                                                                            size );
//        }
			/* try gnome-mime-foo */
			if ( ! file )
				{
					icon_name[11]='\0'; /* strlen("gnome-mime-")=11 */
					strncat( icon_name, mime_type, ( sep - mime_type ));
					//icon=vfs_load_icon ( icon_theme, icon_name, size );
					file=getDIcon ( NULL, icon_name );
				}
			/* try foo-x-generic */
			if ( ! file )
				{
					strncpy( icon_name, mime_type, ( sep - mime_type ));
					icon_name[(sep - mime_type)]='\0';
					strcat( icon_name, "-x-generic" );
					file=getDIcon ( NULL, icon_name );
					//icon=vfs_load_icon ( icon_theme, icon_name, size );
				}
		}

	GtkIconInfo	*inf=gtk_icon_theme_lookup_icon(defaultTheme,icon_name, 48,GTK_ICON_LOOKUP_GENERIC_FALLBACK);

	if( ! inf )
		return(NULL);
	file1 =strdup( gtk_icon_info_get_filename( inf ));
	gtk_icon_info_free( inf );

	return(file1);

}
