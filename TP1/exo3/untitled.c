
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define HOST "intra.esiee.fr"
#define PORT 80
#define FILE "index.html"

int main (void)
{
   struct hostent *host_address = gethostbyname (HOST);

   if (host_address != NULL)
   {
      int socket_id = socket (AF_INET, SOCK_STREAM, 0);

      if (socket_id != -1)
      {
         struct sockaddr_in sockname;
         /*int optval = 1;

         setsockopt (socket_id, SOL_SOCKET, SO_REUSEADDR, &optval,
                     sizeof optval);
*/
         sockname.sin_family = host_address->h_addrtype;
         sockname.sin_port = htons (PORT);
         memcpy (&sockname.sin_addr.s_addr, host_address->h_addr,
                 host_address->h_length);

         if (connect (socket_id, (struct sockaddr *) &sockname,
                      sizeof (struct sockaddr_in)) != -1)
         {
            char str[1024];
            int l;

            write (socket_id, "GET ", sizeof ("GET ") - 1);
            write (socket_id, FILE, sizeof (FILE) - 1);
            write (socket_id, " HTTP/1.1\r\nHost: ",
                   sizeof (" HTTP/1.1\r\nHost: ") - 1);
            write (socket_id, HOST, sizeof (HOST) - 1);
            write (socket_id, "\r\n\r\n", sizeof ("\r\n\r\n") - 1);

            while ((l = read (socket_id, str, sizeof (str) - 1)))
            {
               str[l] = 0;
               printf ("%s", str);
            }
            shutdown (socket_id, 2);
            close (socket_id);
         }
         else                   /* connect () */
         {
            perror ("connect ()");
         }
      }
      else                      /* socket () */
      {
         perror ("socket ()");
      }
   }
   else                         /* gethostbyname () */
   {
      perror ("gethostbyname ()");
   }

   return 0;
}