#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#define BUF_SIZE 4096

#define MAX_PENDING 5
#define MAX_LINE 256

#define INFO_PORT 5432
#define DATA_PORT 5433
#define max_site_name_size 255
#define max_site_desc_size 255
#define max_station_name_size 255
#define max_station_count 255
#define max_song_name_size 255
#define max_bytes_in_frame_TCP 769
#define max_bytes_in_frame_UDP 769


#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

//Declaring variables globally

GtkWidget	*window0;
GtkWidget	*window1;
GtkWidget	*window2;
GtkWidget	*window3;

GtkWidget	*fixed0;
GtkWidget	*fixed1;
GtkWidget	*fixed2;
GtkWidget	*fixed3;

GtkWidget	*station1;
GtkWidget	*station2;
GtkWidget	*station3;

GtkWidget	*option1_of1;
GtkWidget	*option2_of1;
GtkWidget	*option3_of1;

GtkWidget	*option1_of2;
GtkWidget	*option2_of2;
GtkWidget	*option3_of2;

GtkWidget	*option1_of3;
GtkWidget	*option2_of3;
GtkWidget	*option3_of3;

GtkWidget 	*label0;
GtkWidget 	*label1;
GtkWidget 	*label2;
GtkWidget 	*label3;

GtkWidget 	*label1_of1;
GtkWidget 	*label2_of1;
GtkWidget 	*label3_of1;
GtkWidget 	*label4_of1;
GtkWidget 	*label5_of1;

GtkWidget 	*label1_of2;
GtkWidget 	*label2_of2;
GtkWidget 	*label3_of2;
GtkWidget 	*label4_of2;
GtkWidget 	*label5_of2;

GtkWidget 	*label1_of3;
GtkWidget 	*label2_of3;
GtkWidget 	*label3_of3;
GtkWidget 	*label4_of3;
GtkWidget 	*label5_of3;

GtkBuilder	*builder;

pthread_t gui_thread;

struct station_info_request{
   uint8_t type;
}sireq_t;

struct station_info{
   uint8_t station_number;
   uint8_t station_name_size;
   char station_name[ max_station_name_size ];
   uint32_t multicast_address;
   uint16_t data_port;
   uint16_t info_port;
   uint32_t bit_rate;
}station_t;

struct site_info{
   uint8_t type;
   uint8_t site_name_size;
   char site_name[ max_site_name_size ];
   uint8_t site_desc_size;
   char site_desc[ max_site_desc_size ];
   uint8_t station_count;
   struct station_info station_list [ max_station_count];
}site_t;


struct song_info{
   uint8_t type;
   uint8_t song_name_size;
   char song_name[ max_song_name_size ];
   uint16_t remaining_time_in_sec;
   uint8_t next_song_name_size;
   char next_song_name[ max_song_name_size ];
}song_t;

pthread_t udp_data;
pthread_t udp_info;
pthread_t udp_info;
pthread_t song_play;
pthread_mutex_t lock;

struct site_info site;
int stNum=1,choice,s1,s2;
char *if_name = "wlp1s0";
struct station_info_request req;
struct song_info song;

char *host = "0.0.0.0";
int flag = 1,flag2=1;
int current_window_index = -1;

gchar gchar_song_name[max_song_name_size];
gchar gchar_time_remaining[max_song_name_size];
gchar gchar_next_song_name[max_song_name_size];

char buf[BUF_SIZE];

void * start_gui(){

    while(1){

        if(current_window_index == 1){

            gtk_widget_show(label3_of1);
            gtk_widget_show(label4_of1);
            gtk_widget_show(label5_of1);

            gtk_widget_hide(label3_of2);
            gtk_widget_hide(label4_of2);
            gtk_widget_hide(label5_of2);

            gtk_widget_hide(label3_of3);
            gtk_widget_hide(label4_of3);
            gtk_widget_hide(label5_of3);

            gtk_label_set_text (GTK_LABEL(label3_of1), (const gchar* ) gchar_song_name);
            gtk_label_set_text (GTK_LABEL(label4_of1), (const gchar* ) gchar_time_remaining);
            gtk_label_set_text (GTK_LABEL(label5_of1), (const gchar* ) gchar_next_song_name);
        }
        else if(current_window_index == 2){

            gtk_widget_show(label3_of2);
            gtk_widget_show(label4_of2);
            gtk_widget_show(label5_of2);

            gtk_widget_hide(label3_of1);
            gtk_widget_hide(label4_of1);
            gtk_widget_hide(label5_of1);

            gtk_widget_hide(label3_of3);
            gtk_widget_hide(label4_of3);
            gtk_widget_hide(label5_of3);

            gtk_label_set_text (GTK_LABEL(label3_of2), (const gchar* ) gchar_song_name);
            gtk_label_set_text (GTK_LABEL(label4_of2), (const gchar* ) gchar_time_remaining);
            gtk_label_set_text (GTK_LABEL(label5_of2), (const gchar* ) gchar_next_song_name);
        }
        else if(current_window_index == 3){

            gtk_widget_show(label3_of3);
            gtk_widget_show(label4_of3);
            gtk_widget_show(label5_of3);

            gtk_widget_hide(label3_of1);
            gtk_widget_hide(label4_of1);
            gtk_widget_hide(label5_of1);

            gtk_widget_hide(label3_of2);
            gtk_widget_hide(label4_of2);
            gtk_widget_hide(label5_of2);

            gtk_label_set_text (GTK_LABEL(label3_of3), (const gchar* ) gchar_song_name);
            gtk_label_set_text (GTK_LABEL(label4_of3), (const gchar* ) gchar_time_remaining);
            gtk_label_set_text (GTK_LABEL(label5_of3), (const gchar* ) gchar_next_song_name);
        }

        usleep(1000000);
    }
}

void my_itoa(int x, char *str){

    int i = 0;

    if(x == 0){

        str[0] = '0';
        return ;
    }

    while(x){

        str[i++] = x%10+'0';
        x/=10;
    }

    int j, temp;

    for(j=0;j<i/2;j++){

        temp = str[j];
        str[j] = str[i-1-j];
        str[i-1-j] = temp;
    }
}

void clear_buf(){

    int i;

    for (i = 0; i < BUF_SIZE; i++)
        buf[i] = '\0';
}

void recvFile(FILE *file)
{
    fwrite((void *)&buf, sizeof(buf),1,file);
}

void *play(void *args){


    char human_ip[max_site_name_size];
    char cmd[256];
    inet_ntop(AF_INET, &(site.station_list[stNum-1].multicast_address), human_ip, INET_ADDRSTRLEN);

    struct timespec tim1,tim2;
    tim1.tv_sec = 5;
    tim1.tv_nsec = 0 ;
    nanosleep(&tim1,&tim2);

    strcpy(cmd,"ffplay -i ");
    strcat(human_ip,".mp4");
    strcat(cmd,human_ip);
    system(cmd);

}
void * udp_song(void *args){
            struct sockaddr_in sin;
            struct ifreq ifr;
            int len;
            char human_ip[max_site_name_size];
            struct ip_mreq mcast_req;
            struct sockaddr_in mcast_saddr;
            socklen_t mcast_saddr_len;

          if ((s1 = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("receiver: socket");
            exit(1);
          }

          memset((char *)&sin, 0, sizeof(sin));
          sin.sin_family = AF_INET;
          sin.sin_addr.s_addr = htonl(INADDR_ANY);
          sin.sin_port = htons(site.station_list[stNum-1].data_port);
          memset(&ifr, 0, sizeof(ifr));
          strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);

          if ((setsockopt(s1, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr))) < 0){
              perror("receiver: setsockopt() error");
              close(s1);
              exit(1);
            }

          int MegaByte = 1;
          if ((setsockopt(s1, SOL_SOCKET, SO_RCVBUF, (void *)&MegaByte, sizeof(MegaByte))) < 0){
              perror("receiver: setsockopt() buf error");
              close(s1);
              exit(1);
          }

          if ((bind(s1, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
            perror("receiver: bind()");
            exit(1);
          }
          mcast_req.imr_multiaddr.s_addr = site.station_list[stNum-1].multicast_address;
          mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

          if ((setsockopt(s1, IPPROTO_IP, IP_ADD_MEMBERSHIP,(void*) &mcast_req, sizeof(mcast_req))) < 0) {
            perror("mcast join receive: setsockopt()");
            exit(1);
          }

          inet_ntop(AF_INET, &(site.station_list[stNum-1].multicast_address), human_ip, INET_ADDRSTRLEN);
          strcat(human_ip,".mp4");

          FILE *fp;
          fp = fopen(human_ip,"wb+");

           memset(&mcast_saddr, 0, sizeof(mcast_saddr));
           mcast_saddr_len = sizeof(mcast_saddr);
            while(1){
                pthread_mutex_lock(&lock);
                pthread_mutex_unlock(&lock);

                if(!flag){
                    flag2=0;
                    if ((len = recvfrom(s1, buf, BUF_SIZE, 0, (struct sockaddr*)&mcast_saddr,&mcast_saddr_len)) < 0) {
                         perror("receiver: recvfrom()");
                         exit(1);
                    }
                    recvFile(fp);
                    clear_buf();
                }else if(flag2==0){
                    flag2=1;
                    fp = fopen(human_ip,"wb+");
                }

               struct timespec tim1,tim2;
                tim1.tv_sec = 0;
                tim1.tv_nsec = 1 ;
                nanosleep(&tim1,&tim2);
            }

}

void * udp_song_info(void *args){
        struct sockaddr_in sin;
        struct ifreq ifr;
        int len;
        struct ip_mreq mcast_req;
        struct sockaddr_in mcast_saddr;
        socklen_t mcast_saddr_len;
          if ((s2 = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("receiver: socket");
            exit(1);
          }

          memset((char *)&sin, 0, sizeof(sin));
          sin.sin_family = AF_INET;
          sin.sin_addr.s_addr = htonl(INADDR_ANY);
          sin.sin_port = htons(site.station_list[stNum-1].info_port);
          memset(&ifr, 0, sizeof(ifr));
          strncpy(ifr.ifr_name , if_name, sizeof(if_name)-1);

          if ((setsockopt(s2, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr))) < 0){
              perror("receiver: setsockopt() error");
              close(s2);
              exit(1);
            }

          int MegaByte = 1;
          if ((setsockopt(s2, SOL_SOCKET, SO_RCVBUF, (void *)&MegaByte,sizeof(MegaByte))) < 0){
              perror("receiver: setsockopt() buf error");
              close(s2);
              exit(1);
          }

          if ((bind(s2, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
            perror("receiver: bind()");
            exit(1);
          }
          mcast_req.imr_multiaddr.s_addr = site.station_list[stNum-1].multicast_address;
          mcast_req.imr_interface.s_addr = htonl(INADDR_ANY);

          if ((setsockopt(s2, IPPROTO_IP, IP_ADD_MEMBERSHIP,(void*) &mcast_req, sizeof(mcast_req))) < 0) {
            perror("mcast join receive: setsockopt()");
            exit(1);
          }
          while(1){
              pthread_mutex_lock(&lock);
              pthread_mutex_unlock(&lock);

              if ((len = recvfrom(s2, &song, sizeof(song), 0, (struct sockaddr*)&mcast_saddr,&mcast_saddr_len)) < 0) {
                         perror("receiver: recvfrom()");
                         exit(1);
              }

              char song_append[] = "SONG: ";

              strcpy(gchar_song_name, song_append);
              strcat(gchar_song_name, song.song_name);

              char time_append[] = "TIME REMAINING: ";
              char time_s_append[] = "s";
              char time_str[10];

              strcpy(gchar_time_remaining, time_append);
              my_itoa(song.remaining_time_in_sec, time_str);
              strcat(gchar_time_remaining, time_str);
              strcat(gchar_time_remaining, time_s_append);

              char next_song_append[] = "NEXT SONG: ";

              strcpy(gchar_next_song_name, next_song_append);
              strcat(gchar_next_song_name, song.next_song_name);

              printf("song name: %s\n",song.song_name);
              printf("remaining time: %d\n",song.remaining_time_in_sec);
              printf("next song name:%s\n",song.next_song_name);

               struct timespec tim1,tim2;
                tim1.tv_sec = 0;
                tim1.tv_nsec = 1 ;
                nanosleep(&tim1,&tim2);

          }
}

void choose_station(int index){

    stNum = index;
    return ;
}

void tcp(){

        struct hostent *hp;
        struct sockaddr_in sin;
        char human_ip[max_site_name_size];
        int s,len;
        hp = gethostbyname(host);
        if (!hp) {
          fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
          exit(1);
        }

        bzero((char *)&sin, sizeof(sin));
        sin.sin_family = AF_INET;
        bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
        sin.sin_port = htons(INFO_PORT);

        if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
          perror("simplex-talk: socket");
          exit(1);
        }


        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
          {
            perror("simplex-talk: connect");
            close(s);
            exit(1);
          }


        req.type = 1;

        send(s, &req, sizeof(req), 0);
        len = recv(s,&site,sizeof(site),0);
        printf("  -->  %s  <--\n",site.site_name);
        printf("%s\n",site.site_desc);
        printf("Number of channels: %d\n\n",site.station_count);
        int i;
        for(i = 0; i < site.station_count; i++){
            printf("Station Number: %d\n", site.station_list[i].station_number);
            printf("Station Name: %s\n", site.station_list[i].station_name);
            inet_ntop(AF_INET, &(site.station_list[i].multicast_address), human_ip, INET_ADDRSTRLEN);
            printf("MultiCast Address: %s\n",human_ip);
            printf("Data Port: %d\n", site.station_list[i].data_port);
            printf("Info Port: %d\n", site.station_list[i].info_port);
            printf("Bit Rate: %d\n\n", site.station_list[i].bit_rate);
        }

}

void start_change_station(){

    tcp();

    return ;
}

void create_threads(){

    pthread_create(&udp_data, NULL, &udp_song,NULL);
    pthread_create(&song_play, NULL, &play,NULL);
    pthread_create(&udp_info,NULL,&udp_song_info,NULL);

    return ;
}

void delete_threads(){

    pthread_cancel(udp_data);
    pthread_cancel(udp_info);
    pthread_cancel(song_play);
    close(s1);
    close(s2);

    return ;
}

void close_ffplay(){
    system("sudo pkill ffplay");
}

int main(int argc, char * argv[]){

    // init Gtk
	gtk_init(&argc, &argv);

//---------------------------------------------------------------------
// establishing contact with xml code in order to adjust widget settings
//---------------------------------------------------------------------

	builder = gtk_builder_new_from_file ("part1.glade");

	window0 = GTK_WIDGET(gtk_builder_get_object(builder, "window0"));
	window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
	window3 = GTK_WIDGET(gtk_builder_get_object(builder, "window3"));

	gtk_widget_hide(window1);
	gtk_widget_hide(window2);
	gtk_widget_hide(window3);

	g_signal_connect(window0, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window1, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window3, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

	fixed0 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed0"));
	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
	fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3"));

	station1 = GTK_WIDGET(gtk_builder_get_object(builder, "station1"));
	station2 = GTK_WIDGET(gtk_builder_get_object(builder, "station2"));
	station3 = GTK_WIDGET(gtk_builder_get_object(builder, "station3"));

	label0 = GTK_WIDGET(gtk_builder_get_object(builder, "label0"));
	label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));
	label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2"));
	label3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3"));

	label1_of1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1_of1"));
	label2_of1 = GTK_WIDGET(gtk_builder_get_object(builder, "label2_of1"));
    label3_of1 = GTK_WIDGET(gtk_builder_get_object(builder, "label3_of1"));
    label4_of1 = GTK_WIDGET(gtk_builder_get_object(builder, "label4_of1"));
    label5_of1 = GTK_WIDGET(gtk_builder_get_object(builder, "label5_of1"));

	label1_of2 = GTK_WIDGET(gtk_builder_get_object(builder, "label1_of2"));
	label2_of2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2_of2"));
    label3_of2 = GTK_WIDGET(gtk_builder_get_object(builder, "label3_of2"));
    label4_of2 = GTK_WIDGET(gtk_builder_get_object(builder, "label4_of2"));
    label5_of2 = GTK_WIDGET(gtk_builder_get_object(builder, "label5_of2"));

	label1_of3 = GTK_WIDGET(gtk_builder_get_object(builder, "label1_of3"));
	label2_of3 = GTK_WIDGET(gtk_builder_get_object(builder, "label2_of3"));
	label3_of3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3_of3"));
    label4_of3 = GTK_WIDGET(gtk_builder_get_object(builder, "label4_of3"));
    label5_of3 = GTK_WIDGET(gtk_builder_get_object(builder, "label5_of3"));

	gtk_widget_show(window0);

    start_change_station();

    pthread_create(&gui_thread, NULL, &start_gui,NULL);

    gtk_main();

    flag=0;
    if (argc>=2) {
        host = argv[1];
    }
    else {
        fprintf(stderr, "usage: simplex-talk host\n");
        exit(1);
    }

    if(argc == 3) {
        if_name = argv[2];
    }
    else
      if_name = "wlo1";


    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init has failed\n");
        return 1;
    }


      while(1){
          printf("1.Start/Change Station\n");
          printf("2.Resume\n");
          printf("3.Pause\n");
          printf("4.Stop\n");
          printf("5.Exit\n");
          printf("Enter your choice:");
          scanf("%d",&choice);
          switch(choice){
            case 1:

                start_change_station();

                break;

            case 2:
                if(flag){

                 create_threads();
                 flag=0;
                }
                break;

            case 3:
                if(!flag){

                 flag=1;
                 delete_threads();
                }
                break;

            case 4:

                delete_threads();
                close_ffplay();
                break;

            case 5:

                delete_threads();
                close_ffplay();
                exit(1);
                break;

            default:
                printf("Invalid choice");
          }
      }

  return 0;
}

void	on_station1_clicked (GtkButton *b) {
		  gtk_widget_hide(window0);
		  gtk_widget_show(window1);
          choose_station(1);
          current_window_index = 1;
	}

void	on_option1_of1_clicked (GtkButton *b) {
		  gtk_widget_hide(label2_of1);
		  gtk_widget_show(label1_of1);
		  gtk_label_set_text (GTK_LABEL(label1_of1), (const gchar* ) "Playing STATION-1");

          if(flag){

             create_threads();
             flag=0;
          }
	}

void	on_option2_of1_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of1);
		  gtk_widget_show(label2_of1);
		  gtk_label_set_text (GTK_LABEL(label2_of1), (const gchar* ) "STATION-1 Paused");

          if(!flag){

             flag=1;
             delete_threads();
          }
	}

void	on_option3_of1_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of1);
		  gtk_widget_hide(label2_of1);
		  gtk_widget_hide(window1);
		  gtk_widget_show(window0);
          start_change_station();
	}


void	on_station2_clicked (GtkButton *b) {
		  gtk_widget_hide(window0);
		  gtk_widget_show(window2);
          choose_station(2);
          current_window_index = 2;
	}

void	on_option1_of2_clicked (GtkButton *b) {
		  gtk_widget_hide(label2_of2);
		  gtk_widget_show(label1_of2);
          gchar out_str[] = "THIS IS FOR TEST";
		  gtk_label_set_text (GTK_LABEL(label1_of2), (const gchar* ) out_str);

          if(flag){

             create_threads();
             flag=0;
          }
	}

void	on_option2_of2_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of2);
		  gtk_widget_show(label2_of2);
		  gtk_label_set_text (GTK_LABEL(label2_of2), (const gchar* ) "STATION-2 Paused");

          if(!flag){

             flag=1;
             delete_threads();
          }
	}

void	on_option3_of2_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of2);
		  gtk_widget_hide(label2_of2);
		  gtk_widget_hide(window2);
		  gtk_widget_show(window0);
          start_change_station();
	}







void	on_station3_clicked (GtkButton *b) {
		  gtk_widget_hide(window0);
		  gtk_widget_show(window3);
          choose_station(3);
          current_window_index = 3;
	}

void	on_option1_of3_clicked (GtkButton *b) {
		  gtk_widget_hide(label2_of3);
		  gtk_widget_show(label1_of3);
		  gtk_label_set_text (GTK_LABEL(label1_of3), (const gchar* ) "Playing STATION-3");

          if(flag){

             create_threads();
             flag=0;
          }
	}

void	on_option2_of3_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of3);
		  gtk_widget_show(label2_of3);
		  gtk_label_set_text (GTK_LABEL(label2_of3), (const gchar* ) "STATION-3 Paused");

          if(!flag){

             flag=1;
             delete_threads();
          }
	}

void	on_option3_of3_clicked (GtkButton *b) {
		  gtk_widget_hide(label1_of3);
		  gtk_widget_hide(label2_of3);
		  gtk_widget_hide(window3);
		  gtk_widget_show(window0);
          start_change_station();
	}

