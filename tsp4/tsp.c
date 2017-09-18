#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define maxN 50 
#define MAXCOST 1000000
#define RANDBOX 1001

int tour[maxN], besttour[maxN];
int datx[maxN], daty[maxN], distmatrix[maxN][maxN];
int ncount, bestlen = maxN * MAXCOST;

int main (int ac, char **av);
void dist_build();
int dist(int i, int j);
void tour_swap(int i, int j);
void permute(int k, int tourlen);
int mst(int count);

int main (int ac, char **av)
{
    int i;
    if (ac != 3) {
        printf ("Usage: %s number_of_cities random_seed\n", *av);
        return 1;
    }

    ncount = atoi(av[1]);
    srandom(atoi(av[2]));
    dist_build();
    for (i = 0; i < ncount; i++) tour[i] = i;
    permute(ncount-1,0);

    printf ("Best Len = %d\n", bestlen);
    printf ("Optimal Tour: ");
    for (i = 0; i < ncount; i++) printf ("%d ", besttour[i]);
    printf ("\n");
    return 0;
}

void dist_build()
{
    int i, j, k;
    double t1, t2;
    for (i = 0; i < ncount; i++) {
        datx[i] = random() % RANDBOX;
        daty[i] = random() % RANDBOX;
    }

    for (i = 0; i < ncount; i++) {
        for (j = 0; j < i; j++) {
            t1 = (double) (datx[i] - datx[j]);
            t2 = (double) (daty[i] - daty[j]);
            k = (int) (sqrt (t1*t1 + t2*t2) + 0.5);
            distmatrix[i][j] = distmatrix[j][i] = k;
        }
        distmatrix[i][i] = 0;
    }
}

int dist(int i, int j)
{
    return distmatrix[i][j];
}

void tour_swap(int i, int j)
{
    int temp;
    temp = tour[i]; tour[i] = tour[j]; tour[j] = temp;
}

void permute(int k, int tourlen)
{
    int i;
    if (tourlen+mst(k+1) >= bestlen) return;

    if (k == 1) {
        tourlen += (dist(tour[0],tour[1]) + dist(tour[ncount-1],tour[0]));
        if (tourlen < bestlen) {
            bestlen = tourlen;
            for (i = 0; i < ncount; i++) besttour[i] = tour[i];
        }
    } else {
        for (i = 0; i < k; i++) {
            tour_swap(i,k-1);
            permute(k-1, tourlen+dist(tour[k-1],tour[k]));
            tour_swap(i,k-1);
        }
    }
}

int mst(int count)  /* Adopted from Bentley, Unix Review 1996 */
{
    int i, m, mini, newcity, mindist, thisdist, len = 0;
    int pcity[maxN], pdist[maxN];
    if (count <= 1) return 0;

    for (i = 0; i < count; i++) {
        pcity[i] = tour[i];  pdist[i] = MAXCOST;
    }
    if (count != ncount) pcity[count++] = tour[ncount-1]; 

    newcity = pcity[count-1];
    for (m = count-1; m > 0; m--) {
        mindist = MAXCOST;
        for (i = 0; i < m; i++) {
            thisdist = dist(pcity[i],newcity);
            if (thisdist < pdist[i]) pdist[i] = thisdist;
            if (pdist[i] < mindist) { mindist = pdist[i]; mini = i; }
        }
        newcity = pcity[mini];
        len += mindist;
        pcity[mini] = pcity[m-1];  pdist[mini] = pdist[m-1];
    }
    return len;
}
