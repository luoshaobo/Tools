/* regtool.cc

   Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
   2009, 2010, 2011 Red Hat Inc.

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
//#include <getopt.h>
#include "getopt.h"
#include <locale.h>
#define WINVER 0x0502
#include <windows.h>
//#include <sys/cygwin.h>
//#include <cygwin/version.h>
//#include "loadlib.h"
#include "stdafx.h"
#include "TK_Tools.h"
#include "TK_RegKey.h"

using namespace TK_Tools;

#define MAX_BUF_LEN             1024
#if defined(WINCE)
#define BUFSIZ                  1024
int errno = 0;
#endif
#define ssize_t                 signed __int64

#define DEFAULT_KEY_SEPARATOR '\\'

#define REG_AUTO -1

int value_type = REG_AUTO;

char key_sep = DEFAULT_KEY_SEPARATOR;

#define LIST_KEYS        0x01
#define LIST_VALS        0x02
#define LIST_ALL        (LIST_KEYS | LIST_VALS)

static char *prog_name;

static struct option longopts[] =
{
  {"binary", no_argument, NULL, 'b' },
  {"dword", no_argument, NULL, 'd' },
  {"dword-be", no_argument, NULL, 'D' },
#if !defined(WINCE)
  {"expand-string", no_argument, NULL, 'e' },
#endif // #if !defined(WINCE)
  {"help", no_argument, NULL, 'h' },
  {"integer", no_argument, NULL, 'i' },
  {"keys", no_argument, NULL, 'k'},
  {"list", no_argument, NULL, 'l'},
  {"multi-string", no_argument, NULL, 'm'},
  {"none", no_argument, NULL, 'n' },
  {"postfix", no_argument, NULL, 'p'},
  {"quiet", no_argument, NULL, 'q'},
  {"qword", no_argument, NULL, 'Q' },
  {"string", no_argument, NULL, 's'},
  {"verbose", no_argument, NULL, 'v'},
  {"version", no_argument, NULL, 'V'},
#if !defined(WINCE)
  {"wow64", no_argument, NULL, 'w'},
  {"wow32", no_argument, NULL, 'W'},
#endif // #if !defined(WINCE)
  {"hex", no_argument, NULL, 'x'},
  {"key-separator", required_argument, NULL, 'K'},
  {NULL, 0, NULL, 0}
};

static char opts[] = "bdDehiklmnpqQsvVwWxK:";

const char *types[] =
{
  "REG_NONE",
  "REG_SZ",
  "REG_EXPAND_SZ",
  "REG_BINARY",
  "REG_DWORD",
  "REG_DWORD_BIG_ENDIAN",
  "REG_LINK",
  "REG_MULTI_SZ",
  "REG_RESOURCE_LIST",
  "REG_FULL_RESOURCE_DESCRIPTOR",
  "REG_RESOURCE_REQUIREMENTS_LIST",
  "REG_QWORD",
};

int listwhat = 0;
int postfix = 0;
int verbose = 0;
int quiet = 0;
int hex = 0;
DWORD wow64 = 0;
char **argv;

HKEY key;
wchar_t *value;

static void
usage (FILE *where = stderr)
{
  fprintf (where, ""
  "Usage: %s [OPTION] ACTION KEY [data...]\n"
  "\n"
  "View or edit the Win32 registry\n"
  "\n", prog_name);
  if (where == stdout)
    {
      fprintf (where, ""
      "Actions:\n"
      "\n"
      " add KEY\\SUBKEY             add new SUBKEY\n"
      " check KEY                  exit 0 if KEY exists, 1 if not\n"
      " get KEY\\VALUE              prints VALUE to stdout\n"
      " list KEY                   list SUBKEYs and VALUEs\n"
      " remove KEY                 remove KEY\n"
      " set KEY\\VALUE [data ...]   set VALUE\n"
      " unset KEY\\VALUE            removes VALUE from KEY\n"
#if !defined(WINCE)
      " load KEY\\SUBKEY PATH       load hive from PATH into new SUBKEY\n"
      " unload KEY\\SUBKEY          unload hive and remove SUBKEY\n"
      " save KEY\\SUBKEY PATH       save SUBKEY into new hive PATH\n"
#endif // #if !defined(WINCE)
      " test                        for test purpose only\n"
      "\n");
      fprintf (where, ""
      "Options for 'list' Action:\n"
      "\n"
      " -k, --keys           print only KEYs\n"
      " -l, --list           print only VALUEs\n"
      " -p, --postfix        like ls -p, appends '\\' postfix to KEY names\n"
      "\n"
      "Options for 'get' Action:\n"
      "\n"
      " -b, --binary         print data as printable hex bytes\n"
      " -n, --none           print data as stream of bytes as stored in registry\n"
      " -x, --hex            print numerical data as hex numbers\n"
      "\n"
      "Options for 'set' Action:\n"
      "\n"
      " -b, --binary         set type to REG_BINARY (hex args or '-')\n"
      " -d, --dword          set type to REG_DWORD\n"
      " -D, --dword-be       set type to REG_DWORD_BIG_ENDIAN\n"
#if !defined(WINCE)
      " -e, --expand-string  set type to REG_EXPAND_SZ\n"
#endif // #if !defined(WINCE)
      " -i, --integer        set type to REG_DWORD\n"
      " -m, --multi-string   set type to REG_MULTI_SZ\n"
      " -n, --none           set type to REG_NONE\n"
      " -Q, --qword          set type to REG_QWORD\n"
      " -s, --string         set type to REG_SZ\n"
      "\n"
      "Options for 'set' and 'unset' Actions:\n"
      "\n"
      " -K<c>, --key-separator[=]<c>  set key-value separator to <c> instead of '\\'\n"
      "\n"
      "Other Options:\n"
      "\n"
      " -i             enter interactive mode, exiting with sub command \"exit\"\n"
      " -E             enable extended expression which may be supported by printf()\n"
      " -h, --help     output usage information and exit\n"
      " -q, --quiet    no error output, just nonzero return if KEY/VALUE missing\n"
      " -v, --verbose  verbose output, including VALUE contents when applicable\n"
#if !defined(WINCE)
      " -w, --wow64    access 64 bit registry view (ignored on 32 bit Windows)\n"
      " -W, --wow32    access 32 bit registry view (ignored on 32 bit Windows)\n"
#endif // #if !defined(WINCE)
      " -V, --version  output version information and exit\n"
      "\n");
      fprintf (where, ""
//	      "KEY is in the format [host]\\prefix\\KEY\\KEY\\VALUE, where host is optional\n"
//	      "remote host in either \\\\hostname or hostname: format and prefix is any of:\n"
      "KEY is in the format \\prefix\\KEY\\KEY\\VALUE\n"
      "  root     HKCR  HKEY_CLASSES_ROOT (local only)\n"
      "  config   HKCC  HKEY_CURRENT_CONFIG (local only)\n"
      "  user     HKCU  HKEY_CURRENT_USER (local only)\n"
      "  machine  HKLM  HKEY_LOCAL_MACHINE\n"
      "  users    HKU   HKEY_USERS\n"
      "\n"
//	      "If the keyname starts with a forward slash ('/'), the forward slash is used\n"
//	      "as separator and the backslash can be used as escape character.\n"
      );
//	      fprintf (where, ""
//	      "Example:\n"
//	      "%s list '/machine/SOFTWARE/Classes/MIME/Database/Content Type/audio\\/wav'\n\n", prog_name);
    }
  if (where == stderr)
    fprintf (where,
    "ACTION is one of add, check, get, list, remove, set, unset, load, unload, save\n"
    "\n"
    "Try `%s --help' for more information.\n", prog_name);
  exit (where == stderr ? 1 : 0);
}

#define CYGWIN_VERSION_DLL_MAJOR            (1 * 1000 + 7)
#define CYGWIN_VERSION_DLL_MINOR            25

static void
print_version ()
{
  printf ("regtool (cygwin) %d.%d.%d\n"
          "Registry tool\n"
          "Copyright (C) 2000 - %s Red Hat, Inc.\n"
          "This is free software; see the source for copying conditions.  There is NO\n"
          "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
          CYGWIN_VERSION_DLL_MAJOR / 1000,
          CYGWIN_VERSION_DLL_MAJOR % 1000,
          CYGWIN_VERSION_DLL_MINOR,
          strrchr (__DATE__, ' ') + 1);
}

void
Fail (unsigned int rv)
{
#if !defined(WINCE)
  char *buf;
#else
  //wchar_t *buf;
#endif // #if !defined(WINCE)

  if (!quiet)
    {
#if !defined(WINCE)
      /*FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER
                     | FORMAT_MESSAGE_FROM_SYSTEM,
                     0, rv, 0, (CHAR *) & buf, 0, 0);*/
      FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER        // NOTE: modified by Shaobo
                     | FORMAT_MESSAGE_FROM_SYSTEM,
                     0, rv, 0, (CHAR *) & buf, 0, 0);
      fprintf (stderr, "Fail(): nLastError (%d): %s\n", rv, buf);
      LocalFree (buf);
#else
      // FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER        // NOTE: modified by Shaobo
                     // | FORMAT_MESSAGE_FROM_SYSTEM,
                     // 0, rv, 0, (wchar_t *) & buf, 0, 0);
      fprintf (stderr, "Fail(): nLastError (%d)\n", rv);
      //LocalFree (buf);
#endif // #if !defined(WINCE)
    }
  exit (1);
}

static struct
{
  const char *string;
  HKEY key;
} wkprefixes[] =
{
  {"root", HKEY_CLASSES_ROOT},
  {"HKCR", HKEY_CLASSES_ROOT},
  {"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
#if !defined(WINCE)
  {"config", HKEY_CURRENT_CONFIG},
  {"HKCC", HKEY_CURRENT_CONFIG},
  {"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG},
#endif // #if !defined(WINCE)
  {"user", HKEY_CURRENT_USER},
  {"HKCU", HKEY_CURRENT_USER},
  {"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
  {"machine", HKEY_LOCAL_MACHINE},
  {"HKLM", HKEY_LOCAL_MACHINE},
  {"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
  {"users", HKEY_USERS},
  {"HKU", HKEY_USERS},
  {"HKEY_USERS", HKEY_USERS},
  {0, 0}
};

void
translate (char *key)
{
#define isodigit(c) (strchr("01234567", c))
#define tooct(c)    ((c)-'0')
#define tohex(c)    (strchr(_hs,tolower(c))-_hs)
  static char _hs[] = "0123456789abcdef";

  char *d = key;
  char *s = key;
  char c;

  while (*s)
    {
      if (*s == '\\')
        switch (*++s)
          {
          case 'a':
            *d++ = '\007';
            break;
          case 'b':
            *d++ = '\b';
            break;
          case 'e':
            *d++ = '\033';
            break;
          case 'f':
            *d++ = '\f';
            break;
          case 'n':
            *d++ = '\n';
            break;
          case 'r':
            *d++ = '\r';
            break;
          case 't':
            *d++ = '\t';
            break;
          case 'v':
            *d++ = '\v';
            break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
            c = tooct (*s);
            if (isodigit (s[1]))
              {
                c = (c << 3) | tooct (*++s);
                if (isodigit (s[1]))
                  c = (c << 3) | tooct (*++s);
              }
            *d++ = c;
            break;
          case 'x':
            if (!isxdigit (s[1]))
              c = '0';
            else
              {
                c = tohex (*++s);
                if (isxdigit (s[1]))
                  c = (c << 4) | tohex (*++s);
              }
            *d++ = c;
            break;
          default:                /* before non-special char: just add the char */
            *d++ = *s;
            break;
          }
      else if (*s == '/')
        *d++ = '\\';
      else
        *d++ = *s;
      ++s;
    }
  *d = '\0';
}

void
find_key (int howmanyparts, REGSAM access, int option = 0)
{
  HKEY base;
  int rv;
  char *n = argv[0], *e, *h, c;
  char* host = NULL;
  int i;
  size_t len;

  if (*n == '/')
    translate (n);
  if (*n != '\\')
    {
      /* expect host:/key/value format */
      host = (char*) malloc (strlen (n) + 1);
      host[0] = host [1] = '\\';
      for (e = n, h = host + 2; *e && *e != ':'; e++, h++)
        *h = *e;
      *h = 0;
      n = e + 1;
      if (*n == '/')
        translate (n);
    }
  else if (n[0] == '\\' && n[1] == '\\')
    {
      /* expect //host/key/value format */
      host = (char*) malloc (strlen (n) + 1);
      host[0] = host[1] = '\\';
      for (e = n + 2, h = host + 2; *e && *e != '\\'; e++, h++)
        *h = *e;
      *h = 0;
      n = e;
    }
  while (*n != '\\')
    n++;
  *n++ = 0;
  for (e = n; *e && *e != '\\'; e++);
  c = *e;
  *e = 0;
  for (i = 0; wkprefixes[i].string; i++)
    if (strcmp (wkprefixes[i].string, n) == 0)
      break;
  if (!wkprefixes[i].string)
    {
      fprintf (stderr, "Unknown key prefix.  Valid prefixes are:\n");
      for (i = 0; wkprefixes[i].string; i++)
        fprintf (stderr, "\t%s\n", wkprefixes[i].string);
      exit (1);
    }

  n = e;
  *e = c;
  while (*n && *n == '\\')
    n++;
  e = n + strlen (n);
  if (howmanyparts > 1)
    {
      while (n < e && *e != key_sep)
        e--;
      if (*e != key_sep)
        {
          key = wkprefixes[i].key;
          if (value)
            free (value);
          len = mbstowcs (NULL, n, 0) + 1;
          value = (wchar_t *) malloc (len * sizeof (wchar_t));
          mbstowcs (value, n, len);
          return;
        }
      else
        {
          *e = 0;
          if (value)
            free (value);
          len = mbstowcs (NULL, e + 1, 0) + 1;
          value = (wchar_t *) malloc (len * sizeof (wchar_t));
          mbstowcs (value, e + 1, len);
        }
    }
  if (host)
    {
#if !defined(WINCE)
      rv = RegConnectRegistryA (host, wkprefixes[i].key, &base);
      if (rv != ERROR_SUCCESS)
        Fail (rv);
      free (host);
#endif // #if !defined(WINCE)
    }
  else
    base = wkprefixes[i].key;

  if (n[0] == 0)
    key = base;
  else
    {
      len = mbstowcs (NULL, n, 0) + 1;
      //wchar_t name[len];                    // NOTE: modified by Shaobo
      char *name = n;
      //mbstowcs (name, n, len);
      if (access)
        {
#if !defined(WINCE)
          rv = RegOpenKeyExA (base, name, 0, access | wow64, &key);
#else
      rv = RegOpenKeyExW (base, str2wstr(name).c_str(), 0, access | wow64, &key);
#endif // #if !defined(WINCE)
          
          if (option && (rv == ERROR_SUCCESS || rv == ERROR_ACCESS_DENIED))
            {
              /* reopen with desired option due to missing option support in
                 RegOpenKeyE */
              /* FIXME: may create the key in rare cases (e.g. access denied
                 in parent) */
              HKEY key2;
#if !defined(WINCE)
              if (RegCreateKeyExA (base, name, 0, NULL, option, access | wow64,
                                  NULL, &key2, NULL)
#else
          if (RegCreateKeyExW (base, str2wstr(name).c_str(), 0, NULL, option, access | wow64,
                                  NULL, &key2, NULL)
#endif // #if !defined(WINCE)
                  == ERROR_SUCCESS)
                {
                  if (rv == ERROR_SUCCESS)
                    RegCloseKey (key);
                  key = key2;
                  rv = ERROR_SUCCESS;
                }
            }
          if (rv != ERROR_SUCCESS)
            Fail (rv);
        }
      else if (argv[1])
        {
#if !defined(WINCE)
//          ssize_t len = cygwin_conv_path (CCP_POSIX_TO_WIN_W, argv[1], NULL, 0);
//          wchar_t win32_path[len];
//          cygwin_conv_path (CCP_POSIX_TO_WIN_W, argv[1], win32_path, len);
      char *win32_path = argv[1];
          rv = RegLoadKeyA (base, name, win32_path);
          if (rv != ERROR_SUCCESS)
            Fail (rv);
          if (verbose)
            printf ("key %ls loaded from file %ls\n", name, win32_path);
#endif // #if !defined(WINCE)
        }
      else
        {
#if !defined(WINCE)
          rv = RegUnLoadKeyA (base, name);
          if (rv != ERROR_SUCCESS)
            Fail (rv);
          if (verbose)
            printf ("key %ls unloaded\n", name);
#endif // #if !defined(WINCE)
        }
    }
}


int
cmd_list ()
{
  DWORD num_subkeys, maxsubkeylen, num_values, maxvalnamelen, maxvaluelen;
  DWORD maxclasslen;
  wchar_t *subkey_name, *value_name, *class_name, *vd;
  unsigned char *value_data;
  DWORD i, j, m, n, t;
  int v;

  find_key (1, KEY_READ);
  RegQueryInfoKeyW (key, 0, 0, 0, &num_subkeys, &maxsubkeylen, &maxclasslen,
                    &num_values, &maxvalnamelen, &maxvaluelen, 0, 0);

  subkey_name = (wchar_t *) malloc ((maxsubkeylen + 1) * sizeof (wchar_t));
  class_name = (wchar_t *) malloc ((maxclasslen + 1) * sizeof (wchar_t));
  value_name = (wchar_t *) malloc ((maxvalnamelen + 1) * sizeof (wchar_t));
  value_data = (unsigned char *) malloc (maxvaluelen + 1);

  if (!listwhat)
    listwhat = LIST_ALL;

  if (listwhat & LIST_KEYS)
    for (i = 0; i < num_subkeys; i++)
      {
        m = (maxsubkeylen + 1) * sizeof (wchar_t);
        n = (maxclasslen + 1) * sizeof (wchar_t);
        RegEnumKeyExW (key, i, subkey_name, &m, 0, class_name, &n, 0);
        printf ("%ls", subkey_name);
        if (postfix || verbose)
          fputc (key_sep, stdout);

        if (verbose)
          printf (" (%ls)", class_name);

        puts ("");
      }

  if (listwhat & LIST_VALS)
    for (i = 0; i < num_values; i++)
      {
        m = (maxvalnamelen + 1) * sizeof (wchar_t);
        n = maxvaluelen + 1;
        RegEnumValueW (key, i, value_name, &m, 0, &t, (BYTE *) value_data, &n);
        value_data[n] = 0;
        if (!verbose)
          printf ("%ls\n", value_name);
        else
          {
            printf ("%ls (%s) = ", value_name, types[t]);
            switch (t)
              {
              case REG_NONE:
              case REG_BINARY:
                for (j = 0; j < 8 && j < n; j++)
                  printf ("%02x ", value_data[j]);
                printf ("\n");
                break;
              case REG_DWORD:
                printf ("0x%08x (%u)\n", *(unsigned int *) value_data,
                        *(unsigned int *) value_data);
                break;
              case REG_DWORD_BIG_ENDIAN:
                v = ((value_data[0] << 24)
                     | (value_data[1] << 16)
                     | (value_data[2] << 8)
                     | (value_data[3]));
                printf ("0x%08x (%d)\n", v, v);
                break;
              case REG_QWORD:
                printf ("0x%016llx (%llu)\n",
                        *(unsigned long long *) value_data,
                        *(unsigned long long *) value_data);
                break;
              case REG_EXPAND_SZ:
              case REG_SZ:
              case REG_LINK:
                printf ("\"%ls\"\n", (wchar_t *) value_data);
                break;
              case REG_MULTI_SZ:
                vd = (wchar_t *) value_data;
                while (vd && *vd)
                  {
                    printf ("\"%ls\"", vd);
                    vd = vd + wcslen (vd) + 1;
                    if (*vd)
                      printf (", ");
                  }
                printf ("\n");
                break;
              default:
                printf ("?\n");
                break;
              }
          }
      }
  return 0;
}

int
cmd_add ()
{
  find_key (2, KEY_ALL_ACCESS);
  HKEY newkey;
  DWORD newtype;
  int rv = RegCreateKeyExW (key, value, 0, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS | wow64, 0, &newkey, &newtype);
  if (rv != ERROR_SUCCESS)
    Fail (rv);

  if (verbose)
    {
      if (newtype == REG_OPENED_EXISTING_KEY)
        printf ("Key %ls already exists\n", value);
      else
        printf ("Key %ls created\n", value);
    }
  return 0;
}

extern "C" {
  LONG  (WINAPI *regDeleteKeyEx)(HKEY, LPCWSTR, REGSAM, DWORD);
}

int
cmd_remove ()
{
  DWORD rv;

  find_key (2, KEY_ALL_ACCESS);
  if (wow64)
    {
#if !defined(WINCE)
           HMODULE mod = LoadLibraryA ("advapi32.dll");
#else
       HMODULE mod = LoadLibraryW (L"coredll.dll");
#endif // #if !defined(WINCE)
      
      if (mod)
#if !defined(WINCE)
        regDeleteKeyEx = (LONG  (WINAPI *)(HKEY, LPCWSTR, REGSAM, DWORD)) GetProcAddress (mod, "RegDeleteKeyExW");
#else    
    regDeleteKeyEx = (LONG  (WINAPI *)(HKEY, LPCWSTR, REGSAM, DWORD)) GetProcAddress (mod, L"RegDeleteKeyExW");
#endif // #if !defined(WINCE)
    }
  if (regDeleteKeyEx)
    rv = (*regDeleteKeyEx) (key, value, wow64, 0);
  else
    rv = RegDeleteKeyW (key, value);
  if (rv != ERROR_SUCCESS)
    Fail (rv);
  if (verbose)
    printf ("subkey %ls deleted\n", value);
  return 0;
}

int
cmd_check ()
{
  find_key (1, KEY_READ);
  if (verbose)
    printf ("key %s exists\n", argv[0]);
  return 0;
}

int
cmd_set ()
{
  int i, n, max_n;
  DWORD v, rv;
  unsigned long llval;
  char *a = argv[1], *data = 0;
  find_key (2, KEY_ALL_ACCESS);

  if (!a)
    usage ();
  if (value_type == REG_AUTO)
    {
      char *e;
      llval = strtoul (a, &e, 0);
      if (a[0] == '%')
        value_type = REG_EXPAND_SZ;
      else if (a[0] && !*e)
        value_type = llval > 0xffffffffULL ? REG_QWORD : REG_DWORD;
      else if (argv[2])
        value_type = REG_MULTI_SZ;
      else
        value_type = REG_SZ;
    }

  switch (value_type)
    {
    case REG_NONE:
    case REG_BINARY:
      for (n = 0; argv[n+1]; n++)
        ;
      if (n == 1 && strcmp (argv[1], "-") == 0)
        { /* read from stdin */
          i = n = 0;
          for (;;)
            {
              if (i <= n)
                {
                  i = n + BUFSIZ;
                  data = (char *) realloc (data, i);
                }
              int r = fread (data+n, 1, i-n, stdin);
              if (r <= 0)
                break;
              n += r;
            }
        }
      else if (n > 0)
        { /* parse hex from argv */
          data = (char *) malloc (n);
          for (i = 0; i < n; i++)
            {
              char *e;
              errno = 0;
              v = strtoul (argv[i+1], &e, 16);
              if (errno || v > 0xff || *e)
                {
                  fprintf (stderr, "Invalid hex constant `%s'\n", argv[i+1]);
                  exit (1);
                }
              data[i] = (char) v;
            }
        }
      rv = RegSetValueExW (key, value, 0, value_type, (const BYTE *) data, n);
      break;
    case REG_DWORD:
      v = strtoul (a, 0, 0);
      rv = RegSetValueExW (key, value, 0, REG_DWORD, (const BYTE *) &v,
                          sizeof (v));
      break;
    case REG_DWORD_BIG_ENDIAN:
      v = strtoul (a, 0, 0);
      v = (((v & 0xff) << 24)
           | ((v & 0xff00) << 8)
           | ((v & 0xff0000) >> 8)
           | ((v & 0xff000000) >> 24));
      rv = RegSetValueExW (key, value, 0, REG_DWORD_BIG_ENDIAN,
                          (const BYTE *) &v, sizeof (v));
      break;
    case REG_QWORD:
      llval = strtoul (a, 0, 0);
      rv = RegSetValueExW (key, value, 0, REG_QWORD, (const BYTE *) &llval,
                          sizeof (llval));
      break;
    case REG_SZ:
    case REG_EXPAND_SZ:
      {
        n = mbstowcs (NULL, a, 0);
        wchar_t w[MAX_BUF_LEN + 1];
        mbstowcs (w, a, n + 1);
        rv = RegSetValueExW (key, value, 0, value_type,
                             (const BYTE *) w, (n + 1) * sizeof (wchar_t));
      }
      break;
    case REG_MULTI_SZ:
      for (i = 1, max_n = 1; argv[i]; i++)
        max_n += mbstowcs (NULL, argv[i], 0) + 1;
      data = (char *) malloc (max_n * sizeof (wchar_t));
      for (i = 1, n = 0; argv[i]; i++)
        n += mbstowcs ((wchar_t *) data + n, argv[i], max_n - n) + 1;
      ((wchar_t *)data)[n] = L'\0';
      rv = RegSetValueExW (key, value, 0, REG_MULTI_SZ, (const BYTE *) data,
                           (n + 1) * sizeof (wchar_t));
      break;
    case REG_AUTO:
      rv = ERROR_SUCCESS;
      break;
    default:
      rv = ERROR_INVALID_CATEGORY;
      break;
    }

  if (data)
    free(data);

  if (rv != ERROR_SUCCESS)
    Fail (rv);

  return 0;
}

int
cmd_unset ()
{
  find_key (2, KEY_ALL_ACCESS);
  DWORD rv = RegDeleteValueW (key, value);
  if (rv != ERROR_SUCCESS)
    Fail (rv);
  if (verbose)
    printf ("value %ls deleted\n", value);
  return 0;
}

int
cmd_get ()
{
  find_key (2, KEY_READ);
  DWORD vtype, dsize, rv;
  PBYTE data;
  wchar_t *vd;

  rv = RegQueryValueExW (key, value, 0, &vtype, 0, &dsize);
  if (rv != ERROR_SUCCESS)
    Fail (rv);
  data = (PBYTE) malloc (dsize + 1);
  rv = RegQueryValueExW (key, value, 0, &vtype, data, &dsize);
  if (rv != ERROR_SUCCESS)
    Fail (rv);
  if (value_type == REG_BINARY)
    {
      for (unsigned i = 0; i < dsize; i++)
        printf ("%02x%c", (unsigned char)data[i],
          (i < dsize-1 ? ' ' : '\n'));
    }
  else if (value_type == REG_NONE)
    fwrite (data, dsize, 1, stdout);
  else
    switch (vtype)
      {
      case REG_NONE:
        printf("<NONE>\n");
        fwrite (data, dsize, 1, stdout);
        break;
      case REG_BINARY:
        printf("<BINARY>\n");
        fwrite (data, dsize, 1, stdout);
        break;
      case REG_DWORD:
        printf("<DWORD>\n");
        printf (hex ? "0x%08x\n" : "%u\n", *(unsigned int *) data);
        break;
      case REG_DWORD_BIG_ENDIAN:
        printf("<DWORD_BIG_ENDIAN>\n");
        rv = ((data[0] << 24)
              | (data[1] << 16)
              | (data[2] << 8)
              | (data[3]));
        printf (hex ? "0x%08x\n" : "%u\n", (unsigned int) rv);
        break;
      case REG_QWORD:
        printf("<QWORD>\n");
        printf (hex ? "0x%016llx\n" : "%llu\n", *(unsigned long long *) data);
        break;
      case REG_SZ:
        printf("<SZ>\n");
        printf ("%ls\n", (wchar_t *) data);
        break;
      case REG_LINK:
        printf("<LINK>\n");
        printf ("%ls\n", (wchar_t *) data);
        break;
      case REG_EXPAND_SZ:
        printf("<EXPAND_SZ>\n");
        if (value_type == REG_EXPAND_SZ)        // hack
          {
#if !defined(WINCE)
            wchar_t *buf;
            DWORD bufsize;
            bufsize = ExpandEnvironmentStringsW ((wchar_t *) data, 0, 0);
            buf = (wchar_t *) malloc (bufsize + 1);
            ExpandEnvironmentStringsW ((wchar_t *) data, buf, bufsize + 1);
            free (data);
            data = (PBYTE) buf;
#endif // #if !defined(WINCE)
          }
        printf ("%ls\n", (wchar_t *) data);
        break;
      case REG_MULTI_SZ:
        printf("<MULTI_SZ>\n");
        vd = (wchar_t *) data;
        while (vd && *vd)
          {
            printf ("%ls\n", vd);
            vd = vd + wcslen (vd) + 1;
          }
        break;
      }
  return 0;
}

int
cmd_load ()
{
  if (!argv[1])
    {
      usage ();
      return 1;
    }
  find_key (1, 0);
  return 0;
}

int
cmd_unload ()
{
  if (argv[1])
    {
      usage ();
      return 1;
    }
  find_key (1, 0);
  return 0;
}

DWORD
set_privilege (const wchar_t *name)
{
    DWORD rv = 0;
#if !defined(WINCE)
  DWORD nError = 0;
  
  TOKEN_PRIVILEGES tp;
  if (!LookupPrivilegeValueW (NULL, name, &tp.Privileges[0].Luid)) {
    nError = GetLastError ();
    goto FAILED;
  }
  tp.PrivilegeCount = 1;
  tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  HANDLE t;
  /* OpenProcessToken does not work here, because main thread has its own
     impersonation token */
  //if (!OpenThreadToken (GetCurrentThread (), TOKEN_ADJUST_PRIVILEGES, FALSE, &t)) {
  //  nError = GetLastError ();
  //  goto FAILED;
  //}
  if (!OpenProcessToken (GetCurrentProcess (), TOKEN_ADJUST_PRIVILEGES, &t)) {
    nError = GetLastError ();
    goto FAILED;
  }
  AdjustTokenPrivileges (t, FALSE, &tp, 0, NULL, NULL);
  rv = GetLastError ();
  nError = rv;
  CloseHandle (t);

FAILED:
  if (nError != 0) {
      printf("*** set_privilege(): nError=%u\n", nError);
  }
#endif // #if !defined(WINCE)
  return rv;
}

int
cmd_save ()
{
#if !defined(WINCE)
  if (!argv[1])
    {
      usage ();
      return 1;
    }
  /* try to set SeBackupPrivilege, let RegSaveKey report the error */
  set_privilege (SE_BACKUP_NAME);
  /* REG_OPTION_BACKUP_RESTORE is necessary to save /HKLM/SECURITY */
  find_key (1, KEY_QUERY_VALUE, REG_OPTION_BACKUP_RESTORE);
  //ssize_t len = cygwin_conv_path (CCP_POSIX_TO_WIN_W, argv[1], NULL, 0);
  //wchar_t win32_path[len];
  //cygwin_conv_path (CCP_POSIX_TO_WIN_W, argv[1], win32_path, len);
  char *win32_path = argv[1];
  DWORD rv = RegSaveKeyA (key, win32_path, NULL);
  if (rv != ERROR_SUCCESS)
    Fail (rv);
  if (verbose)
    printf ("key saved to %ls\n", win32_path);
#endif // #if !defined(WINCE)
  return 0;
}

static struct
{
  const char *name;
  int (*func) ();
} commands[] =
{
  {"list", cmd_list},
  {"add", cmd_add},
  {"remove", cmd_remove},
  {"check", cmd_check},
  {"set", cmd_set},
  {"unset", cmd_unset},
  {"get", cmd_get},
#if !defined(WINCE)
  {"load", cmd_load},
  {"unload", cmd_unload},
  {"save", cmd_save},
#endif // #if !defined(WINCE)
  {0, 0}
};

int
main_core (int argc, char **_argv)
{
  int g;

#if !defined(WINCE)
  setlocale (LC_ALL, "");
#endif // #if !defined(WINCE)

  prog_name = "regtool.exe";

  while ((g = getopt_long (argc, _argv, opts, longopts, NULL)) != EOF)
    switch (g)
        {
        case 'b':
          value_type = REG_BINARY;
          break;
        case 'd':
          value_type = REG_DWORD;
          break;
        case 'D':
          value_type = REG_DWORD_BIG_ENDIAN;
          break;
        case 'e':
          value_type = REG_EXPAND_SZ;
          break;
        case 'k':
          listwhat |= LIST_KEYS;
          break;
        case 'h':
          usage (stdout);
        case 'i':
          value_type = REG_DWORD;
          break;
        case 'l':
          listwhat |= LIST_VALS;
          break;
        case 'm':
          value_type = REG_MULTI_SZ;
          break;
        case 'n':
          value_type = REG_NONE;
          break;
        case 'p':
          postfix++;
          break;
        case 'q':
          quiet++;
          break;
        case 'Q':
          value_type = REG_QWORD;
          break;
        case 's':
          value_type = REG_SZ;
          break;
        case 'v':
          verbose++;
          break;
        case 'V':
          print_version ();
          exit (0);
#if !defined(WINCE)
        case 'w':
          wow64 = KEY_WOW64_64KEY;
          break;
        case 'W':
          wow64 = KEY_WOW64_32KEY;
          break;
#endif // #if !defined(WINCE)
        case 'x':
          hex++;
          break;
        case 'K':
          key_sep = *optarg;
          break;
        default :
          fprintf (stderr, "Try `%s --help' for more information.\n",
                   prog_name);
          return 1;
        }

  if ((_argv[optind] == NULL) || (_argv[optind+1] == NULL))
    usage ();

  argv = _argv + optind;
  int i;
  for (i = 0; commands[i].name; i++)
    if (strcmp (commands[i].name, argv[0]) == 0)
      {
        argv++;
        return commands[i].func ();
      }
  usage ();

  return 0;
}

int force_add_new_key(const std::string &sKeyPath)
{
    int ret = -1;
    LONG nRet;
    unsigned int i;
    HKEY hRootKey = NULL;
    HKEY hLastKey = NULL;
    HKEY hNewKey = NULL;
    DWORD dwDisposition;

    std::vector<std::string> vKeyNodes = SplitString(sKeyPath, "\\");
    if (vKeyNodes.size() < 1) {
        printf("***ERROR: force_add_new_key(): root key is null\n");
        ret = -1;
        goto FAILED;
    }

    for (i = 0; wkprefixes[i].string; i++) {
        if (strcmp (wkprefixes[i].string, vKeyNodes[0].c_str()) == 0) {
            break;
        }
    }
    if (wkprefixes[i].string == NULL) {
        printf("***ERROR: force_add_new_key(): root key is invalid: %s\n", vKeyNodes[0].c_str());
        ret = -1;
        goto FAILED;
    }

    hRootKey = wkprefixes[i].key;
    hLastKey = hRootKey;
    for (i = 1; i < vKeyNodes.size(); i++) {
        if (vKeyNodes[i].empty()) {
            continue;
        }
        
#if !defined(WINCE)
        nRet = RegCreateKeyExA(hLastKey, vKeyNodes[i].c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hNewKey, &dwDisposition);
#else
        nRet = RegCreateKeyExW(hLastKey, str2wstr(vKeyNodes[i]).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hNewKey, &dwDisposition);
#endif // #if !defined(WINCE)
        if (nRet != ERROR_SUCCESS) {
            printf("***ERROR: force_add_new_key(): RegCreateKeyExA() is failed\n");
            Fail(nRet);
            ret = -1;
            goto FAILED;
        }
        if (hLastKey != hRootKey) {
            RegCloseKey(hLastKey);
        }
        hLastKey = hNewKey;
        hNewKey = NULL;
    }

    ret = 0;
FAILED:
    if (hNewKey != NULL) {
        RegCloseKey(hNewKey);
        hNewKey = NULL;
    }
    if (hLastKey != NULL) {
        if (hLastKey != hRootKey) {
            RegCloseKey(hLastKey);
        }
        hLastKey = NULL;
    }
    return ret;
}

bool arrSubKeyNames_greater(const std::wstring &s1, const std::wstring &s2)
{
    bool bRet;

    bRet = CompareNoCase(wstr2str(s1), wstr2str(s2)) < 0;

    return bRet;
}

bool arrKeyValues_greater(const RegKey::KeyNameValuePair &k1, const RegKey::KeyNameValuePair &k2)
{
    bool bRet;

    bRet = CompareNoCase(wstr2str(k1.sKeyName), wstr2str(k2.sKeyName)) < 0;

    return bRet;
}

int dir_all_content_for_key(const std::string &sKeyPath)
{
    int nRet = 0;
    RegKey::ErrCode nErrCode;
    std::vector<std::wstring> arrSubKeyNames;
    std::vector<RegKey::KeyNameValuePair> arrKeyValues;
    unsigned int i;

    if (nRet == 0) {
        nErrCode = RegKey::GetSubKeyNames(arrSubKeyNames, str2wstr(sKeyPath), NULL);
        if (nErrCode != RegKey::EC_OK) {
            fprintf(stderr, "*** ERROR: %S\n", GetWin32ErrMsg(RegKey::GetWin32LastError()).c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        nErrCode = RegKey::GetValues(arrKeyValues, str2wstr(sKeyPath), NULL);
        if (nErrCode != RegKey::EC_OK) {
            fprintf(stderr, "*** ERROR: %S\n", GetWin32ErrMsg(RegKey::GetWin32LastError()).c_str());
            nRet = -1;
        }
    }

    if (nRet == 0) {
        std::sort(arrSubKeyNames.begin(), arrSubKeyNames.end(), arrSubKeyNames_greater);
        for (i = 0; i < arrSubKeyNames.size(); i++) {
            printf("[%S]\n", arrSubKeyNames[i].c_str());
        }

        std::sort(arrKeyValues.begin(), arrKeyValues.end(), arrKeyValues_greater);
        for (i = 0; i < arrKeyValues.size(); i++) {
            printf("%S => %S\n", arrKeyValues[i].sKeyName.c_str(), arrKeyValues[i].oKeyValue.ToStr().c_str());
        }
    }

    return 0;
}

int interactive()
{
    int nRet = 0;
    char *pLine;
#define CMD_LINE_MAX_LEN                1024
    char arrLineBuf[CMD_LINE_MAX_LEN];

    fprintf(stdout, "NOTE: please enter \"exit\" or \"quit\" to exit.\n");

    while (true) {
        fprintf(stdout, "regtool> ");
        pLine = fgets(arrLineBuf, CMD_LINE_MAX_LEN, stdin);
        if (pLine == NULL) {
            continue;
        }

        std::string sLine(pLine);
        sLine = TrimAll(sLine);
        if (sLine == "exit" || sLine == "quit") {
            nRet = 0;
            break;
#ifndef WINCE
        } else if (sLine == "cls" || sLine == "clear") {
            system("cls");
            continue;
#endif // #ifdef WINCE
        }

#ifdef WINCE
        Execute("regtool.exe", sLine, true);
#else
        std::string sCmdLine;
        FormatStr(sCmdLine, "regtool.exe %s", sLine.c_str());
        system(sCmdLine.c_str());
#endif // #ifdef WINCE
    }

    return nRet;
#undef CMD_LINE_MAX_LEN
}

int test(int argc, char **argv)
{
    int nRet = 0;
    char *pLine;
#define CMD_LINE_MAX_LEN                1024
    char arrLineBuf[CMD_LINE_MAX_LEN];

    fprintf(stdout, "NOTE: please enter \"exit\" or \"quit\" to exit.\n");

    while (true) {
        fprintf(stdout, "test> ");
        pLine = fgets(arrLineBuf, CMD_LINE_MAX_LEN, stdin);
        if (pLine == NULL) {
            continue;
        }

        std::string sLine(pLine);
        sLine = TrimAll(sLine);
        if (sLine == "exit" || sLine == "quit") {
            nRet = 0;
            break;
#ifndef WINCE
        } else if (sLine == "cls" || sLine == "clear") {
            system("cls");
            continue;
#endif // #ifdef WINCE
        }

        RegKey::KeyValue oKeyValue;
        bool bSuc = RegKey::KeyValue::FromStr(str2wstr(sLine), oKeyValue);
        if (bSuc) {
            printf("%S\n", oKeyValue.ToStr().c_str());
        } else {
            printf("*** Error: unknown format!\n");
        }
    }

    return nRet;
#undef CMD_LINE_MAX_LEN
}

#define PERSISTENCE_FILE_PATH               TEXT("~regtool.tmp")

static std::string g_sCurrentKeyPrefix = "";

int main (int argc, char **_argv)
{
    int ret = -1;
    int i, j;
    char **_argv_2 = new char *[argc + 1];
    
    for (i = 0; i < argc; i++) {
        _argv_2[i] = new char[std::string(_argv[i]).length() + 1];
        strncpy(_argv_2[i], std::string(_argv[i]).c_str(), std::string(_argv[i]).length());
        _argv_2[i][std::string(_argv[i]).length()] = '\0';
    }
    _argv_2[i] = NULL;

    //
    // test
    //
    if (argc == 2 && std::string(_argv_2[1]) == "test") {
        ret = test(argc, _argv_2);
        goto END1;
    }

    //
    // -i
    //
    if (argc == 2 && std::string(_argv_2[1]) == "-i") {
        ret = interactive();
        goto END1;
    }

    //
    // -E supporting.
    //
    {
        for (i = 0; i < argc; i++) {
            if (std::string(_argv_2[i]) == "-E") {
                break;
            }
        }

        if (i < argc) {
            delete [] _argv_2[i];
            argc--;
            for (j = i; j < argc; j++) {
                _argv_2[j] = _argv_2[j + 1];
            }
            _argv_2[j] = NULL;

            for (i = 0; i < argc; i++) {
                translate(_argv_2[i]);
            }
        }
    }

    //
    // Get saved key prefix.
    //
    GetContentFromFile(PERSISTENCE_FILE_PATH, g_sCurrentKeyPrefix);

    //
    // Process new command: 
    //     cd
    //
    if (argc >= 3) {
        if (CompareNoCase(_argv_2[1], "cd") == 0) {
            if (CompareNoCase(_argv_2[2], ".") == 0) {
                // do nothing
            } else if (CompareNoCase(_argv_2[2], ".\\") == 0) {
                g_sCurrentKeyPrefix += '\\';
            } else if (CompareNoCase(_argv_2[2], "..") == 0) {
                std::string::size_type nPos = g_sCurrentKeyPrefix.rfind('\\');
                if (nPos != std::string::npos) {
                    g_sCurrentKeyPrefix = g_sCurrentKeyPrefix.substr(0, nPos);
                }
            } else if (std::string(_argv_2[2])[0] == '\\') {
                g_sCurrentKeyPrefix = _argv_2[2];
            } else {
                if (g_sCurrentKeyPrefix.length() >= 1) {
                    if (g_sCurrentKeyPrefix[g_sCurrentKeyPrefix.length() - 1] == '\\') {
                        g_sCurrentKeyPrefix += _argv_2[2];
                    } else {
                        g_sCurrentKeyPrefix += '\\';
                        g_sCurrentKeyPrefix += _argv_2[2];
                    }
                } else {
                    g_sCurrentKeyPrefix = '\\';
                    g_sCurrentKeyPrefix += _argv_2[2];
                }
            }
            
            printf("Current key prefix: %s\n", g_sCurrentKeyPrefix.c_str());
            ret = 0;
            goto END;
        }
    } else if (argc >= 2) {
        if (CompareNoCase(_argv_2[1], "cd") == 0) {
            printf("Current key prefix: %s\n", g_sCurrentKeyPrefix.c_str());
            ret = 0;
            goto END;
        }
    }

    //
    // Add key prefix if possible. 
    //
    if (argc >= 3) {
        for (j = 2; j < argc; j++) {
            if (std::string(_argv_2[j])[0] != '-') {    // find the first argument, not the options.
                break;
            }
        }
        if (j < argc) {
            std::string sKey(_argv_2[j]);
            if (sKey[0] != '\\' && sKey[0] != '/') {
                if (sKey == ".") {
                    sKey = g_sCurrentKeyPrefix;
                } else if (sKey == ".\\") {
                    if (g_sCurrentKeyPrefix.length() >= 1) {
                        if (g_sCurrentKeyPrefix[g_sCurrentKeyPrefix.length() - 1] == '\\') {
                            sKey = g_sCurrentKeyPrefix;
                        } else {
                            sKey = g_sCurrentKeyPrefix + '\\';
                        }
                    } else {
                        // do nothing
                    }
                } else {
                    if (g_sCurrentKeyPrefix.length() > 0) {
                        if (g_sCurrentKeyPrefix[g_sCurrentKeyPrefix.length() - 1] == '\\') {
                            sKey = g_sCurrentKeyPrefix + sKey;
                        } else {
                            sKey = g_sCurrentKeyPrefix + "\\" + sKey;
                        }
                    }
                }
            }
        
            delete [] _argv_2[j];
            _argv_2[j] = new char[sKey.length() + 1];
            strncpy(_argv_2[j], sKey.c_str(), sKey.length());
            _argv_2[j][sKey.length()] = '\0';
        }
    }

    //
    // Process new command: 
    //     add -p <new_key_path>
    //
    if (argc >= 4) {
        if (std::string(_argv_2[1]) == "add" && std::string(_argv_2[2]) == "-p") {
            ret = force_add_new_key(_argv_2[3]);
            if (ret == 0) {
                printf("Successful to create key: %s\n", _argv_2[3]);
            } else {
                printf("Failed to create key: %s\n", _argv_2[3]);
            }
            goto END;
        }
    }

    //
    // Process new command: 
    //     dir
    //
    if (argc >= 2) {
        if (std::string(_argv_2[1]) == "dir") {
            dir_all_content_for_key(g_sCurrentKeyPrefix);
            goto END;
        }
    } else if (argc >= 3) {
        if (std::string(_argv_2[1]) == "dir") {
            dir_all_content_for_key(_argv_2[2]);
            goto END;
        }
    }
    
    ret = main_core(argc, _argv_2);
    
END:
    //
    // save key prefix.
    //
    SaveContentToFile(PERSISTENCE_FILE_PATH, g_sCurrentKeyPrefix);

END1:

    for (i = 0; i < argc; i++) {
        delete [] _argv_2[i];
    }
    delete [] _argv_2;
    
    return ret;
}

#if defined(WINCE)
int wmain (int argc, wchar_t **_argv)
{
    int ret = -1;
    int i;
    char **_argv_2 = new char *[argc + 1];
    
    for (i = 0; i < argc; i++) {
        _argv_2[i] = new char[wstr2str(_argv[i]).length() + 1];
        strncpy(_argv_2[i], wstr2str(_argv[i]).c_str(), wstr2str(_argv[i]).length());
        _argv_2[i][wstr2str(_argv[i]).length()] = L'\0';
    }
    _argv_2[i] = NULL;
    
    ret = main(argc, _argv_2);
    
    for (i = 0; i < argc; i++) {
        delete [] _argv_2[i];
    }
    delete [] _argv_2;
    
    return ret;
}
#endif // #if defined(WINCE)
