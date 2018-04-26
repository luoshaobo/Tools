#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define NAMELEN 16

#define errExitEN(en, msg) \
           do { errno = en; perror(msg); exit(EXIT_FAILURE); \
       } while (0)

static void *
threadfunc(void *parm)
{
   for (;;) {
       sleep(1);
   }
   return NULL;
}

int
main(int argc, char **argv)
{
   pthread_t thread;
   int rc;
   char thread_name[NAMELEN];

   rc = pthread_create(&thread, NULL, threadfunc, NULL);
   if (rc != 0)
       errExitEN(rc, "pthread_create");

   // rc = pthread_getname_np(thread, thread_name, NAMELEN);
   // if (rc != 0)
       // errExitEN(rc, "pthread_getname_np");

   printf("Created a thread. Default name is: %s\n", thread_name);
   rc = pthread_setname_np(thread, (argc > 1) ? argv[1] : "yyyyyy");
   if (rc != 0)
       errExitEN(rc, "pthread_setname_np");

   pthread_setname_np(pthread_self(), "xxxxxx");
       
   for (;;) {
       sleep(1);
   }

   // rc = pthread_getname_np(thread, thread_name,
                           // (argc > 2) ? atoi(argv[1]) : NAMELEN);
   // if (rc != 0)
       // errExitEN(rc, "pthread_getname_np");
   // printf("The thread name after setting it is %s.\n", thread_name);

   // rc = pthread_join(thread, NULL);
   // if (rc != 0)
       // errExitEN(rc, "pthread_join");

   printf("Done\n");
   exit(EXIT_SUCCESS);
}