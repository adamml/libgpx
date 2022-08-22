#include <libgpx.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    wchar_t *buffer = 0;
    long len;
    libgpx_GPX *gpx = malloc(sizeof(libgpx_GPX));
    if(gpx != NULL){
        /**FILE *f = fopen("Alpe_d_Huez_road_climb.gpx", "r");*/
        /** FILE *f = fopen("HR100-Course-Clockwise.gpx", "r"); */
        FILE *f = fopen("Hardrock_100.gpx", "r");

        if (f)
        {
            fseek (f, 0, SEEK_END);
            len = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (len);
            if (buffer)
            {
                fread (buffer, 1, len, f);
            }
            fclose (f);
        }

        if (buffer){
            int gpx_type_count_status = libgpx_count_gpx_types((char *)buffer, gpx);

            if(gpx_type_count_status > 0){
                libgpx_GPXPoint *points = malloc(sizeof(libgpx_GPXPoint) * gpx->n_GPXPoint);
                libgpx_GPXLink *links = malloc(sizeof(libgpx_GPXLink) * gpx->n_GPXLink);
                libgpx_GPXPointCollection *collections = malloc(sizeof(libgpx_GPXPointCollection) * gpx->n_GPXPointCollection);

                gpx->points = points;
                gpx->collections = collections;
                gpx->links = links;

                libgpx_parse_gpx((unsigned char*)buffer, gpx);

                printf("%s\t%s\t%s\t%i\t%s\n", gpx->metadata.name, gpx->metadata.copyright.author, gpx->metadata.copyright.license, gpx->metadata.copyright.year, gpx->metadata.author.name);

                int i = 0;
                while(i < gpx->n_GPXPointCollection){
                    printf("%i\t%i\t%i\t%s\t%s\n", gpx->collections[i].id, gpx->collections[i].collection_type, gpx->collections[i].member_of, gpx->collections[i].name, gpx->collections[i].type);
                    ++i;
                }
                i = 0;
                while(i < gpx->n_GPXLink){
                    printf("%i\t%s\n", gpx->links[i].member_of, gpx->links[i].href);
                    ++i;
                }

                i = 0;
                while(i < gpx->n_GPXPoint){
                    printf("%i\t%s\t%f\t%f\t%f\n",gpx->points[i].member_of, gpx->points[i].name, gpx->points[i].latitude, gpx->points[i].longitude, gpx->points[i].elevation);
                    ++i;
                }
            }   
        }
    }
    return 0;
}