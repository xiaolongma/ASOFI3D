/*------------------------------------------------------------------------
 * exchange of particle velocities at grid boundaries between processors
 * when using the standard staggered grid
 *
 *  ----------------------------------------------------------------------*/

#include "fd.h"
#include "globvar.h"


double exchange_s_acoustic(int nt, float *** sxx,
		float *** bufferlef_to_rig, float *** bufferrig_to_lef,
		float *** buffertop_to_bot, float *** bufferbot_to_top,
		float *** bufferfro_to_bac, float *** bufferbac_to_fro) {

	extern int NX, NY, NZ, POS[4], NPROCX, NPROCY, NPROCZ, BOUNDARY, MYID, FDORDER, LOG, INDEX[7];
	extern const int TAG1,TAG2,TAG3,TAG4,TAG5,TAG6;
	extern FILE *FP;
	extern int OUTNTIMESTEPINFO;

	MPI_Status status;	
	int i, j, k, l, nf1, nf2;
	double time=0.0, time1=0.0, time2=0.0;

	nf1=FDORDER/2;
	nf2=(FDORDER/2)-1;


	if (LOG){
		if ((MYID==0) && ((nt+(OUTNTIMESTEPINFO-1))%OUTNTIMESTEPINFO)==0) time1=MPI_Wtime();}


	/* top-bottom -----------------------------------------------------------*/	

	if (POS[2]!=0)	/* no boundary exchange at top of global grid */
		for (i=1;i<=NX;i++){
			for (k=1;k<=NZ;k++){

				/* storage of top of local volume into buffer */
				for (l=1;l<=FDORDER/2;l++)
					buffertop_to_bot[i][k][l]  = sxx[l][i][k];

			}
		}


	if (POS[2]!=NPROCY-1)	/* no boundary exchange at bottom of global grid */
		for (i=1;i<=NX;i++){
			for (k=1;k<=NZ;k++){


				/* storage of bottom of local volume into buffer */

				for (l=1;l<=(FDORDER/2-1);l++)
					bufferbot_to_top[i][k][l]  = sxx[NY-l+1][i][k];

			}
		}

	MPI_Sendrecv_replace(&buffertop_to_bot[1][1][1],NX*NZ*nf1,MPI_FLOAT,INDEX[3],TAG5,INDEX[4],TAG5,MPI_COMM_WORLD,&status);
	MPI_Sendrecv_replace(&bufferbot_to_top[1][1][1],NX*NZ*nf2,MPI_FLOAT,INDEX[4],TAG6,INDEX[3],TAG6,MPI_COMM_WORLD,&status);

	if (POS[2]!=NPROCY-1)	/* no boundary exchange at bottom of global grid */
		for (i=1;i<=NX;i++){
			for (k=1;k<=NZ;k++){

				for (l=1;l<=FDORDER/2;l++)
					sxx[NY+l][i][k] = buffertop_to_bot[i][k][l];



			}
		}

	if (POS[2]!=0)	/* no boundary exchange at top of global grid */
		for (i=1;i<=NX;i++){
			for (k=1;k<=NZ;k++){
				for (l=1;l<=(FDORDER/2-1);l++)
					sxx[1-l][i][k] = bufferbot_to_top[i][k][l];

			}
		}






	/* left-right -----------------------------------------------------------*/	



	if ((BOUNDARY) || (POS[1]!=0))	/* no boundary exchange at left edge of global grid */
		for (j=1;j<=NY;j++){
			for (k=1;k<=NZ;k++){

				/* storage of left edge of local volume into buffer */
				for (l=1;l<=FDORDER/2;l++)
					bufferlef_to_rig[j][k][l] =  sxx[j][l][k];
			}
		}


	if ((BOUNDARY) || (POS[1]!=NPROCX-1))	/* no boundary exchange at right edge of global grid */
		for (j=1;j<=NY;j++){
			for (k=1;k<=NZ;k++){
				/* storage of right edge of local volume into buffer */
				for (l=1;l<=(FDORDER/2-1);l++)
					bufferrig_to_lef[j][k][l] =  sxx[j][NX-l+1][k];
			}
		}

	MPI_Sendrecv_replace(&bufferlef_to_rig[1][1][1],NY*NZ*nf1,MPI_FLOAT,INDEX[1],TAG1,INDEX[2],TAG1,MPI_COMM_WORLD,&status);
	MPI_Sendrecv_replace(&bufferrig_to_lef[1][1][1],NY*NZ*nf2,MPI_FLOAT,INDEX[2],TAG2,INDEX[1],TAG2,MPI_COMM_WORLD,&status);

	if ((BOUNDARY) || (POS[1]!=NPROCX-1))	/* no boundary exchange at right edge of global grid */
		for (j=1;j<=NY;j++){
			for (k=1;k<=NZ;k++){


				for (l=1;l<=FDORDER/2;l++)
					sxx[j][NX+l][k] = bufferlef_to_rig[j][k][l];

			}
		}

	if ((BOUNDARY) || (POS[1]!=0))	/* no boundary exchange at left edge of global grid */
		for (j=1;j<=NY;j++){
			for (k=1;k<=NZ;k++){
				for (l=1;l<=(FDORDER/2-1);l++)
					sxx[j][1-l][k] = bufferrig_to_lef[j][k][l];

			}
		}






	/* front-back -----------------------------------------------------------*/
	if ((BOUNDARY) || (POS[3]!=0))	/* no boundary exchange at front side of global grid */
		for (i=1;i<=NX;i++){
			for (j=1;j<=NY;j++){


				/* storage of front side of local volume into buffer */
				for (l=1;l<=(FDORDER/2);l++)
					bufferfro_to_bac[j][i][l]  = sxx[j][i][l];

			}
		}


	if ((BOUNDARY) || (POS[3]!=NPROCZ-1))	/* no boundary exchange at back side of global grid */
		for (i=1;i<=NX;i++){
			for (j=1;j<=NY;j++){

				/* storage of back side of local volume into buffer */
				for (l=1;l<=(FDORDER/2-1);l++)
					bufferbac_to_fro[j][i][l]  = sxx[j][i][NZ-l+1];

			}
		}

	MPI_Sendrecv_replace(&bufferfro_to_bac[1][1][1],NX*NY*nf1,MPI_FLOAT,INDEX[5],TAG3,INDEX[6],TAG3,MPI_COMM_WORLD,&status);
	MPI_Sendrecv_replace(&bufferbac_to_fro[1][1][1],NX*NY*nf2,MPI_FLOAT,INDEX[6],TAG4,INDEX[5],TAG4,MPI_COMM_WORLD,&status);

	if ((BOUNDARY) || (POS[3]!=NPROCZ-1))	/* no boundary exchange at back side of global grid */
		for (i=1;i<=NX;i++){
			for (j=1;j<=NY;j++){

				for (l=1;l<=(FDORDER/2);l++)
					sxx[j][i][NZ+l] = bufferfro_to_bac[j][i][l];


			}
		}


	if ((BOUNDARY) || (POS[3]!=0))	/* no boundary exchange at front side of global grid */
		for (i=1;i<=NX;i++){
			for (j=1;j<=NY;j++){


				for (l=1;l<=(FDORDER/2-1);l++)
					sxx[j][i][1-l] = bufferbac_to_fro[j][i][l];


			}
		}

	if (LOG)
		if ((MYID==0) && ((nt+(OUTNTIMESTEPINFO-1))%OUTNTIMESTEPINFO)==0){
			time2=MPI_Wtime();
			time=time2-time1;
			fprintf(FP," Real time for stress tensor exchange: \t\t %4.2f s.\n",time);
		}
	return time;


}




















