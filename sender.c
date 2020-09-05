/* CSD 304 Computer Networks, Fall 2016
   Lab 4, Sender
   Team:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>


#define DATA_PORT 5433
#define INFO_PORT 5432
#define BUF_SIZE 4096

#define MAX_PENDING 5
#define MAX_LINE 256

#define max_site_name_size 255
#define max_site_desc_size 255
#define max_station_name_size 255
#define max_station_count 255
#define max_song_name_size 255
#define max_bytes_in_frame_TCP 769
#define max_bytes_in_frame_UDP 769


struct station_info_request{
    uint8_t type; // 1
}; // 1

struct station_info{
    uint8_t station_number;
    uint8_t station_name_size;
    char station_name[ max_station_name_size ];
    uint32_t multicast_address;
    uint16_t data_port;
    uint16_t info_port;
    uint32_t bit_rate;
}; // 1+1+255+4+2+2+4 = 269

struct site_info{
    uint8_t type;  // 10
    uint8_t site_name_size;
    char site_name[ max_site_name_size ];
    uint8_t site_desc_size;
    char site_desc[ max_site_desc_size ];
    uint8_t station_count;
    struct station_info station_list [ max_station_count];
}; // 1+1+255+1+255+1+ 255*269 = 69109


struct song_info{
    uint8_t type; // 12
    uint8_t song_name_size;
    char song_name[ max_song_name_size ];
    uint16_t remaining_time_in_sec;
    uint8_t next_song_name_size;
    char next_song_name[ max_song_name_size ];
}; // 1+1+255+2+1+255 = 516

char buf[BUF_SIZE];

struct site_info si;

int cnt=0,number_of_stations;

pthread_t recieve_t;
pthread_mutex_t lock;

uint32_t to_uint32_t(char s[]){
    uint32_t add=0,tmp;

    char a[4],i,j=0;

    for(i=0;s[i+j]!='.';i++)
        a[i]=s[i+j];
    a[i]=0;
    j+=i;
    j++;
    sscanf(a, "%u", &tmp);
    add=add*256+tmp;

    for(i=0;s[i+j]!='.';i++)
        a[i]=s[i+j];
    a[i]=0;
    j+=i;
    j++;
    sscanf(a, "%u", &tmp);
    add=add*256+tmp;


    for(i=0;s[i+j]!='.';i++)
        a[i]=s[i+j];
    a[i]=0;
    j+=i;
    j++;
    sscanf(a, "%u", &tmp);
    add=add*256+tmp;

    for(i=0;s[i+j]!=0;i++)
        a[i]=s[i+j];
    a[i]=0;
    j+=i;
    j++;
    sscanf(a, "%u", &tmp);
    add=add*256+tmp;

    return add;
}

void to_stringa(uint32_t multicast_address,char s1[]){

    int i=0,j=0,k,temp,val=(1<<24);
    char arr[4];
    while(val!=0){
        temp=(multicast_address/val)%256;
        val/=256;
        i++;
        k=0;
        while(temp>0){
            arr[k++]=(char)(temp%10+'0');
            temp/=10;
        }
        while(k--){
            s1[j++]=arr[k];
        }
        s1[j++]='.';
    }
    s1[j-1]=0;
}


void sendFile(FILE* fp1)
{
	fread((void *)&buf, sizeof(buf),1,fp1);
}

int get_file_size(FILE *file){

    int size = 0;

    fseek(file, 0, SEEK_END);

    size = ftell(file);

    fseek(file, 0, SEEK_SET);

    return size;
}

int get_times(int size){

    if(size % BUF_SIZE == 0)
        return size/BUF_SIZE;

    return size/BUF_SIZE + 1;
}

void clear_buf(){

    int i;

    for (i = 0; i < BUF_SIZE; i++)
        buf[i] = '\0';
}

void * station(void *args){

    int station_index = (int) args;

    int i, j, songs_count;
    pthread_mutex_lock(&lock);

    printf("NUMBER OF SONGS TO SEND: \n");

    scanf("%d", &songs_count);

    char songs_list[songs_count][max_song_name_size];

    char append_ffmpeg[] = "ffmpeg -i ";

    char append_mpegts[] = " -f mpegts ";

    char append_stream[] = "_stream.mp4";

    char append_bps[] = "_bps.mp4";

    char rate_str[10];

    sprintf(rate_str,"%d",si.station_list[station_index].bit_rate);

    char append_rate[] = " -c:v libx264 -b:v ";

    char append_rate_1[] = " -maxrate 1M -bufsize 700k ";

    strcat(append_rate,rate_str);

    strcat(append_rate,append_rate_1);

    for(i = 0; i < songs_count; i ++){

        while(1){

            printf("ENTER THE %d SONG NAME: \n", i+1);

            fflush(stdout);

            scanf("%s",&songs_list[i]);

            for(j = 0; j < max_song_name_size; j ++){

                if(songs_list[i][j] == '\n'){

                    songs_list[i][j] = '\0';
                    break;
                }
            }

            FILE *file_e;

	        file_e = fopen(songs_list[i], "rb");

            if (file_e == NULL){

                printf("FILE DOES NOT EXIST: \n PLEASE ENTER AGAIN: \n");

                fflush(stdout);
            }
            else {

                printf("FILE %s EXIST \n",songs_list[i]);

                fflush(stdout);

                fclose(file_e);

                break;
            }
        }

        char change_bps[max_song_name_size];

        memset((char *)&change_bps, 0, sizeof(change_bps));

        strcat(change_bps,append_ffmpeg);

        strcat(change_bps,songs_list[i]);

        strcat(change_bps,append_rate);

        char final_song_name[max_song_name_size];

        memset((char *)&final_song_name, 0, sizeof(final_song_name));

        for(j = 0; ; j ++){

            final_song_name[j] = songs_list[i][j];

            if(songs_list[i][j] == '.'){

                final_song_name[j] = '\0';
                break;
            }
        }

        strcat(final_song_name,append_bps);

        strcat(change_bps,final_song_name);

        system(change_bps);

        strncpy(songs_list[i], final_song_name, max_song_name_size);


        char streamable_string[max_song_name_size];

        memset((char *)&streamable_string, 0, sizeof(streamable_string));

        strcat(streamable_string,append_ffmpeg);

        strcat(streamable_string,songs_list[i]);

        strcat(streamable_string,append_mpegts);

        for(j = 0; ; j ++){

            final_song_name[j] = songs_list[i][j];

            if(songs_list[i][j] == '.'){

                final_song_name[j] = '\0';
                break;
            }
        }

        strcat(final_song_name,append_stream);

        strcat(streamable_string,final_song_name);

        system(streamable_string);

        strncpy(songs_list[i], final_song_name, max_song_name_size);

    }

    pthread_mutex_unlock(&lock);
    cnt++;
    while(cnt<number_of_stations)usleep(100000);


    static int *current_song_index, *time_left;

    current_song_index = mmap(NULL, sizeof *current_song_index, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    time_left = mmap(NULL, sizeof *time_left, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid_t fork_for_data_info = fork();

    //int fork_for_data_info = 1;

    int s, len, times, size, count;

    struct sockaddr_in sin;

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {

        perror("SERVER: SOCKET");
        exit(1);
    }

    memset((char *)&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;

    char mcast_addr[100];

//    to_stringa(si.station_list[station_index].multicast_address,mcast_addr);

    sin.sin_addr.s_addr = si.station_list[station_index].multicast_address;

    if(fork_for_data_info != 0){

        sin.sin_port = htons(si.station_list[station_index].data_port);
    }
    else{

        sin.sin_port = htons(si.station_list[station_index].info_port);
    }

    if(fork_for_data_info != 0){

        FILE *file_test;

        while(1){

            for(i = 0; i < songs_count; i ++) {

	            FILE *file = fopen(songs_list[i], "rb");

                size = get_file_size(file);

                times = get_times(size);

                *current_song_index = i;

                *time_left = size/si.station_list[station_index].bit_rate;

                printf("TIMES : %d\n", times);

                long long delay = (1000000*BUF_SIZE)/si.station_list[station_index].bit_rate;

                while(times > 0){
                    //printf("%d %d\n",*time_left,times);

                    clear_buf();

                    sendFile(file);

                    if ((len = sendto(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, sizeof(sin))) == -1) {

                        perror("SENDER: SEND TO");
                        exit(1);
                    }

                    times -= 1;

                    *time_left = (times*BUF_SIZE)/si.station_list[station_index].bit_rate;

                    usleep(delay);

                }

                printf("SENT SONG: %d\n", i+1);

                fflush(stdout);


                fclose(file);
            }
        }
    }
    else{

        struct song_info current_song_info;

        while(1) {

            current_song_info.type = 12;

            current_song_info.song_name_size = strlen(songs_list[*current_song_index]);

            strcpy (current_song_info.song_name,songs_list[*current_song_index]);

            current_song_info.remaining_time_in_sec = *time_left;

            current_song_info.next_song_name_size = strlen(songs_list[(*current_song_index + 1) % songs_count]);

            strcpy (current_song_info.song_name, songs_list[(*current_song_index + 1) % songs_count]);

            if ((len = sendto(s, &current_song_info, sizeof(current_song_info), 0, (struct sockaddr *)&sin, sizeof(sin))) == -1) {

                perror("SENDER: SEND TO");
                exit(1);
            }

            printf("SECONDS LEFT: %d\n",*time_left);

            printf("CURRENT SONG INDEX: %d %d\n",*current_song_index,station_index);

            usleep(1000000);
        }
    }
}


int main(int argc, char * argv[]){

    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int len;
    int s, new_s;
    char str[INET_ADDRSTRLEN];
    char multicast_add[16];

    struct station_info_request sir;

    sir.type=0;

    if (pthread_mutex_init(&lock, NULL) != 0){

        printf("\n mutex init has failed\n");
        return 1;
    }

    pthread_mutex_lock(&lock);

    printf("Enter site name:");

    fflush(stdout);

    scanf("%[^\n]",si.site_name); getchar();

    si.site_name_size=strlen(si.site_name);

    printf("Enter site description:");
    fflush(stdout);
    scanf("%[^\n]",si.site_desc,si.site_desc); getchar();
    si.site_desc_size=strlen(si.site_desc);

    printf("Enter number of stations :");
    fflush(stdout);
    scanf("%hhu",&si.station_count); getchar();
    number_of_stations=si.station_count;

    printf("%s , %s , %d ",si.site_name,si.site_desc,si.station_count);

    pthread_t thread_station[si.station_count];

    for(int i=0;i<si.station_count;i++){
        printf("Enter information for station %d\n",i);

        si.station_list[i].station_number=i+1;

        printf("Enter station name :");
        fflush(stdout);
        scanf("%[^\n]",si.station_list[i].station_name); getchar();
        si.station_list[i].station_name_size=strlen(si.station_list[i].station_name);

        printf("Enter multicast address :");
        fflush(stdout);
        scanf("%s",multicast_add); getchar();
        si.station_list[i].multicast_address=inet_addr(multicast_add);

        si.station_list[i].data_port=DATA_PORT;
        si.station_list[i].info_port=INFO_PORT;

        si.station_list[i].bit_rate=500000;

        pthread_create(&thread_station[i], NULL, &station,(void *)i);

    }


    pthread_mutex_unlock(&lock);


    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(INFO_PORT);

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        for(int i=0;i<si.station_count;i++)
            pthread_cancel(thread_station[i]);

        perror("simplex-talk: socket");
        exit(1);
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, INFO_PORT);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
         for(int i=0;i<si.station_count;i++)
        pthread_cancel(thread_station[i]);

        perror("simplex-talk: bind");
        exit(1);
    }
    else
        printf("Server bind done.\n");

    listen(s, MAX_PENDING);

    while(1) {
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
             for(int i=0;i<si.station_count;i++)
        pthread_cancel(thread_station[i]);

            perror("simplex-talk: accept");
            close(new_s);
            close(s);
          exit(1);
        }

        printf("Server Listening.\n");
        len = recv(new_s, &sir, sizeof(sir), 0);
        if(sir.type==1)
            send(new_s, &si ,sizeof(si), 0);
        close(new_s);
    }

    return 0;
}

