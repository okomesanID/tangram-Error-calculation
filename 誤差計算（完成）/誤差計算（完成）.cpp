#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<GL/glut.h>
#define QP 9
#define AP 6
#define BP 4

void drawCircle(double x, double y, double radius)
{
	int i;	double PI = 3.14159;	int CIRCLE_REC = 60;

	glBegin(GL_POLYGON);
	for (i = 0; i < CIRCLE_REC; i++)
		glVertex2f(x + radius*cos((2.*PI*i) / CIRCLE_REC),
		y + radius*sin((2.*PI*i) / CIRCLE_REC));
	glEnd();
}

//図形の座標番号A
void render_stringA(float x, float y, const char* string)
{
	float z = -1.0f;
	glRasterPos3f(x, y, z);
	char*p = (char*)string;
	while (*p != '\0')glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
}

//図形の座標番号B
void render_stringB(float x, float y, const char* string)
{
	float z = -1.0f;
	glRasterPos3f(x, y, z);
	char*p = (char*)string;
	while (*p != '\0')glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
}


void display()
{
	//図形乱数動作
	int dummy, i = 0, j = 0;
	double x[4], y[4];//基準線
	double Qx[QP], Qy[QP];	//シルエット座標
	double ax[AP], ay[AP];	//五角形座標
	double bx[BP], by[BP];  //三角形座標
	double rrA[AP - 1], rrB[BP - 1];	//三平方の定理
	double thetaA = 0, thetaB = 0;	//角度の初期パラメータ（乱数）
	double theta1[AP], theta2[BP];	//各座標の角度
	double thetaAC, thetaAD[AP - 1], thetaBC, thetaBD[BP - 1]; //角度動作用　
	double disAx[AP - 1], disAy[AP - 1], disBx[BP - 1], disBy[BP - 1];	//0座標から各点の座標距離

	//内外接続動作
	int cn1[AP - 1], cnn1 = 0, nan1 = 0, maxAxx = 0, minAxx = 0, maxAyy = 0, minAyy = 0; //交差カウント
	int cn2[BP - 1], cnn2 = 0, nan2 = 0, maxBxx = 0, minBxx = 0, maxByy = 0, minByy = 0; //交差カウント
	double abAx, abAy;	//鉛直線座標
	double abBx, abBy;	//鉛直線座標
	double saAx, saAy; //交差座標までの内部差
	double saBx, saBy; //交差座標までの内部差
	double cdAx = 0, cdAy = 0;	//交差座標
	double cdBx = 0, cdBy = 0;	//交差座標
	double maxAx, minAx, maxAy, minAy;	//内部三角形の大小値
	double maxBx, minBx, maxBy, minBy;	//内部三角形の大小値
	double LineBx[AP], LineBy[AP], CroBx[AP], CroBy[AP];	//線分計算
	double LineAx[BP], LineAy[BP], CroAx[BP], CroAy[BP];	//線分計算

	//近接接続動作
	int gcn2[BP - 1], gcng2 = 0, ggg = 0;//近接用
	double gAx = 0, gAy = 0, gBx = 0, gBy = 0;	//重心点
	double gdisBx[BP], gdisBy[BP];	//重心との座標距離（pieace2だけ）

	//正規化動作
	int norm1 = 0, norm2 = 0;	//正規化スイッチ
	double QminAx, QminAy, QminBx, QminBy;	//正規化点
	double axx[AP], ayy[AP], bxx[BP], byy[BP];	//正規化のための座標保存
	double r = 0.1;

	//誤差計量
	int gi, fi;
	int QAn = 0, QBn = 0, An = 0, Bn = 0, Aerror = 0, Amatch = 0, Berror = 0, Bmatch = 0; //誤差計量
	double Qxtemp[2], Qytemp[2], Axtemp[2], Aytemp[2], Bxtemp[2], Bytemp[2]; //各図形座標の大小値
	double hyoukaAx = 0, hyoukaAy = 0, hyoukaBx = 0, hyoukaBy = 0;	//評価点
	double h = 0.01, hA1, hA2, hB1, hB2;//解像度
	double Aa, Ab, Ac, Ad; //誤差計量pieace1
	double Ba, Bb, Bc, Bd; //誤差計量pieace2
	double Aratio = 0, Bratio = 0;	//誤差比
	double fr = 0.01; //誤差点の半径

	//乱数座標
	double ran[4], ram[2];	//乱数用
	char str[256];	//座標番号

	srand(100);
	dummy = rand();
	dummy = rand();

	glClearColor(1.0, 1.0, 1.0, 1.0);

	//基準線
	x[0] = -5; y[0] = 0;
	x[1] = 5; y[1] = 0;
	x[2] = 0; y[2] = -5;
	x[3] = 0; y[3] = 5;

	//問題シルエット座標
	Qx[0] = 1.0; Qy[0] = 0.0;
	Qx[1] = 2.0; Qy[1] = 1.1;
	Qx[2] = 2.0; Qy[2] = 2.0;
	Qx[3] = 1.5; Qy[3] = 2.0;
	Qx[4] = 1.5; Qy[4] = 3.1;
	Qx[5] = 0.5; Qy[5] = 2.0;
	Qx[6] = 0.0; Qy[6] = 2.0;
	Qx[7] = 0.0; Qy[7] = 0.0;
	Qx[8] = 1.0; Qy[8] = 0.0;

	//四角形-pieace1
	ax[0] = -1.0-2; ay[0] = -1.0-2;
	ax[1] = 0.0-2; ay[1] = -1.0-2;
	ax[2] = 1.0-2; ay[2] = 0.1-2;
	ax[3] = 1.0-2; ay[3] = 1.0-2;
	ax[4] = -1.0-2; ay[4] = 1.0-2;
	ax[5] = -1.0-2; ay[5] = -1.0-2;

	//三角形-pieace2
	bx[0] = 0.0-2; by[0] = -1.0-2;
	bx[1] = 1.0-2; by[1] = -1.0-2;
	bx[2] = 1.0-2; by[2] = 0.1-2;
	bx[3] = 0.0-2; by[3] = -1.0-2;


	//初期角度乱数
	ran[0] = 0;	ran[1] = 0;
	ran[2] = 0; ran[3] = 0;

	//平行移動スイッチ
	ram[0] = 0; ram[1] = 0;



	//Q図形のx座標比較（誤差計算用）
	Qxtemp[0] = Qx[0];
	Qxtemp[1] = Qx[0];
	for (i = 0; i < QP-1; i++){
		if (Qxtemp[0] > Qx[i])
			Qxtemp[0] = Qx[i];
		if (Qxtemp[1] < Qx[i])
			Qxtemp[1] = Qx[i];
	}
	//F図形のy座標比較（誤差計算用）
	Qytemp[0] = Qy[0];
	Qytemp[1] = Qy[0];
	for (i = 0; i < QP-1; i++){
		if (Qytemp[0] > Qy[i])
			Qytemp[0] = Qy[i];
		if (Qytemp[1] < Qy[i])
			Qytemp[1] = Qy[i];
	}


	//以下アニメーションの描画
	for (int iiii = 1; iiii <= 1000; iiii++){
		//角度変換バグお直し処理
		for (i = 0; i < AP - 2; i++) {
			for (j = i + 1; j < AP - 1; j++) {
				if (fabs(ax[i] - ax[j]) < 0.000001)	ax[j] += 0.000001;
				if (fabs(ay[i] - ay[j]) < 0.000001) ay[j] += 0.000001;
			}
		}
		ax[AP - 1] = ax[0];		ay[AP - 1] = ay[0];

		for (i = 0; i < BP - 2; i++) {
			for (j = i + 1; j < BP - 1; j++) {
				if (fabs(bx[i] - bx[j]) < 0.000001) bx[j] += 0.000001;
				if (fabs(by[i] - by[j]) < 0.000001) by[j] += 0.000001;
			}
		}
		//bx[BP - 1] = bx[0];	by[BP - 1] = by[0];


		double time1 = (double)clock() / CLOCKS_PER_SEC;
		for (;;){
			double time2 = (double)clock() / CLOCKS_PER_SEC;
			if (time2 - time1 > 0.1) break;
		}
		glClear(GL_COLOR_BUFFER_BIT);

		//問題シルエット描画	←順番変えれない
		glBegin(GL_POLYGON);
		glColor3f(1.0, 0.8, 1.0);
		for (i = 0; i < QP; i++)
			glVertex2f(Qx[i], Qy[i]);
		glEnd();

		//基準線	←順番変えれない
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		for (i = 0; i < 4; i++)
			glVertex2f(x[i], y[i]);
		glEnd();



		//pieace1角度
		thetaA = ran[0] * (3.14 / 180);

		//piace1の距離計算
		for (i = 0; i < AP - 1; i++){
			disAx[i] = fabs(ax[0] - ax[i + 1]);
			disAy[i] = fabs(ay[0] - ay[i + 1]);
		}
		for (i = 0; i < 4; i++){
			rrA[i] = sqrt(disAx[i] * disAx[i] + disAy[i] * disAy[i]);
		}

		//pieace1の角度計算
		if (iiii == 1){
			theta1[0] = atan(disAy[0] / disAx[0]);
			if (ax[0]>ax[1] && ay[0] < ay[1]) {  //左上にある
				theta1[0] = 3.14 - theta1[0];
			}
			if (ax[0]>ax[1] && ay[0] > ay[1]){ //左下にある
				theta1[0] += 3.14;
			}
			if (ax[0]<ax[1] && ay[0] > ay[1]) {	//右下にある
				theta1[0] = 2 * 3.14 - theta1[0];
			}

			for (i = 1; i < AP - 1; i++){
				theta1[i] = atan(disAy[i] / disAx[i]);
				if (ax[0]>ax[i + 1] && ay[0] < ay[i + 1]) {  //左上にある
					theta1[i] = 3.14 - theta1[i];
				}
				if (ax[0]>ax[i + 1] && ay[0] > ay[i + 1])  theta1[i] += 3.14;  //左下にある
				if (ax[0]<ax[i + 1] && ay[0] > ay[i + 1])  theta1[i] = 2 * 3.14 - theta1[i];  //右下にある
			}
		}
		else{
			theta1[0] = thetaAC;
			for (i = 1; i < AP - 2; i++){
				theta1[i] = thetaAD[i];
			}
		}

		thetaAC = theta1[0] + thetaA;
		if (thetaAC < 0)  thetaAC += 2 * 3.14;
		for (i = 1; i < AP - 1; i++){
			thetaAD[i] = theta1[i] + thetaA;
			if (thetaAD[i] < 0)  thetaAD[i] += 2 * 3.14;
		}

		//pieace1の角度動作
		ax[1] = ax[0] + rrA[0] * cos(thetaAC);
		ay[1] = ay[0] + rrA[0] * sin(thetaAC);
		for (i = 1; i < AP - 2; i++){
			ax[i + 1] = ax[0] + rrA[i] * cos(thetaAD[i]);
			ay[i + 1] = ay[0] + rrA[i] * sin(thetaAD[i]);
		}
		//pieace1の平行移動
		for (i = 0; i < AP; i++){
			if (ram[0] == 0) ax[i] -= ran[2];
			else ay[i] -= ran[2];

			norm1 += 1; //正規化スイッチ
		}

		//pieace2とpieace1の内外判定
		//※図形の境界線上は図形外としている。これは境界線上が図形の大きさに含まれないためである。
		for (i = 0; i < BP - 1; i++){
			for (j = 0; j < AP - 1; j++){
				if (((ax[j] - 5.0) * (by[i] - ay[j]) + (ay[j] - ay[j]) * (ax[j] - bx[i]))
					* ((ax[j] - 5.0) * (by[i + 1] - ay[j]) + (ay[j] - ay[j]) * (ax[j] - bx[i + 1])) < 0){

					if (((bx[i] - bx[i + 1]) * (ay[j] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[j]))
						* ((bx[i] - bx[i - 1]) * (ay[j] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - 5.0)) < 0){
						cn1[j] += 1;
					}
				}
			}
		}

		//pieace1-内部座標の個数確認
		for (i = 0; i < AP; i++){
			if (cn1[i] % 2 >= 1){
				cnn1 += 1;
			}
		}

		//pieace1が内部に入っている場合の計算
		minAy = 5.0;	maxAy = -5.0;
		minAx = 5.0;	maxAx = -5.0;
		for (i = 0; i < AP - 1; i++){
			if (cn1[i] % 2 >= 1){
				if (ax[i] > maxAx){ //四角形の一番右の座標を特定
					maxAx = ax[i];
					maxAxx = i;
				}
				if (ay[i] > maxAy){ //四角形の一番上の座標を特定
					maxAy = ay[i];
					maxAyy = i;
				}
				if (ax[i] < minAx){ //四角形の一番左の座標を特定
					minAx = ax[i];
					minAxx = i;
				}
				if (ay[i] < minAy){ //四角形の一番下の座標を特定
					minAy = ay[i];
					minAyy = i;
				}
			}
		}

		//移動座標乱数
		nan1 = rand() % 4;

		//pieace1-図形内部の場合の移動処理1
		if (cnn1 == 1){//内部座標が一個の場合
			for (int ii = 0; ii < AP - 1; ii++){
				if (cn1[ii] % 2 >= 1){
					if (nan == 0){	//上に移動
						abAx = ax[ii];	abAy = 5.0;
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[ii] - abAx) * (by[i] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i]))
									* ((ax[ii] - abAx) * (by[i + 1] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[ii] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[ii]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAy = cdAy - ay[ii];
										for (j = 0; j < AP; j++){
											ay[j] = ay[j] + saAy;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 1){	//右に移動
						abAx = 5.0;	abAy = ay[ii];
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[ii] - abAx) * (by[i] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i]))
									* ((ax[ii] - abAx) * (by[i + 1] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[ii] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[ii]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAx = cdAx - ax[ii];
										for (j = 0; j < AP; j++){
											ax[j] = ax[j] + saAx;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 2){	//下に移動
						abAx = ax[ii];	abAy = -5.0;
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[ii] - abAx) * (by[i] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i]))
									* ((ax[ii] - abAx) * (by[i + 1] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[ii] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[ii]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAy = cdAy - ay[ii];
										for (j = 0; j < AP; j++){
											ay[j] = ay[j] + saAy;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 3){	//左に移動
						abAx = -5.0;	abAy = ay[ii];
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[ii])) - (((ay[ii] * abAx) - (ax[ii] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[ii])*(bx[i] - bx[i + 1])) - ((abAx - ax[ii])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[ii] - abAx) * (by[i] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i]))
									* ((ax[ii] - abAx) * (by[i + 1] - ay[ii]) + (ay[ii] - abAy) * (ax[ii] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[ii] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[ii]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAx = cdAx - ax[ii];
										for (j = 0; j < AP; j++){
											ax[j] = ax[j] + saAx;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		//pieace1-図形内部の場合の移動処理2
		else if (cnn1 >= 2){//内部座標が複数個の場合
			for (int ii = 0; ii < BP - 1; ii++){
				if (cn1[ii] % 2 >= 1){
					if (nan == 0){	//上に移動
						abAx = ax[minAyy];	abAy = 5.0;
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[minAyy])) - (((ay[minAyy] * abAx) - (ax[minAyy] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[minAyy])*(bx[i] - bx[i + 1])) - ((abAx - ax[minAyy])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[minAyy])) - (((ay[minAyy] * abAx) - (ax[minAyy] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[minAyy])*(bx[i] - bx[i + 1])) - ((abAx - ax[minAyy])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[minAyy] - abAx) * (by[i] - ay[minAyy]) + (ay[minAyy] - abAy) * (ax[minAyy] - bx[i]))
									* ((ax[minAyy] - abAx) * (by[i + 1] - ay[minAyy]) + (ay[minAyy] - abAy) * (ax[minAyy] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[minAyy] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[minAyy]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAy = cdAy - ay[minAyy];
										for (j = 0; j < AP; j++){
											ay[j] = ay[j] + saAy;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 1){	//右に移動
						abAx = 5.0;	abAy = ay[minAxx];
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[minAxx])) - (((ay[minAxx] * abAx) - (ax[minAxx] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[minAxx])*(bx[i] - bx[i + 1])) - ((abAx - ax[minAxx])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[minAxx])) - (((ay[minAxx] * abAx) - (ax[minAxx] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[minAxx])*(bx[i] - bx[i + 1])) - ((abAx - ax[minAxx])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[minAxx] - abAx) * (by[i] - ay[minAxx]) + (ay[minAxx] - abAy) * (ax[minAxx] - bx[i]))
									* ((ax[minAxx] - abAx) * (by[i + 1] - ay[minAxx]) + (ay[minAxx] - abAy) * (ax[minAxx] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[minAxx] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[minAxx]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAx = cdAx - ax[minAxx];
										for (j = 0; j < AP; j++){
											ax[j] = ax[j] + saAx;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 2){	//下に移動
						abAx = ax[maxAyy];	abAy = -5.0;
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[maxAyy])) - (((ay[maxAyy] * abAx) - (ax[maxAyy] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[maxAyy])*(bx[i] - bx[i + 1])) - ((abAx - ax[maxAyy])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[maxAyy])) - (((ay[maxAyy] * abAx) - (ax[maxAyy] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[maxAyy])*(bx[i] - bx[i + 1])) - ((abAx - ax[maxAyy])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[maxAyy] - abAx) * (by[i] - ay[maxAyy]) + (ay[maxAyy] - abAy) * (ax[maxAyy] - bx[i]))
									* ((ax[maxAyy] - abAx) * (by[i + 1] - ay[maxAyy]) + (ay[maxAyy] - abAy) * (ax[maxAyy] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[maxAyy] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[maxAyy]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAy = cdAy - ay[maxAyy];
										for (j = 0; j < AP; j++){
											ay[j] = ay[j] + saAy;
										}
									}
								}
							}
						}
					}
					else if (nan1 == 3){	//左に移動
						abAx = -5.0;	abAy = by[maxAxx];
						//線分交差判定
						for (i = 0; i < BP - 1; i++){
							LineAx[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAx - ax[maxAxx])) - (((ay[maxAxx] * abAx) - (ax[maxAxx] * abAy))*(bx[i] - bx[i + 1])));
							CroAx[i] = (((abAy - ay[maxAxx])*(bx[i] - bx[i + 1])) - ((abAx - ax[maxAxx])*(by[i] - by[i + 1])));
							LineAy[i] = ((((by[i + 1] * bx[i]) - (bx[i + 1] * by[i]))*(abAy - ay[maxAxx])) - (((ay[maxAxx] * abAx) - (ax[maxAxx] * abAy))*(by[i] - by[i + 1])));
							CroAy[i] = (((abAy - ay[maxAxx])*(bx[i] - bx[i + 1])) - ((abAx - ax[maxAxx])*(by[i] - by[i + 1])));
						}
						for (i = 0; i < BP - 1; i++){
							if (LineAx[i] != 0 && CroAx[i] != 0 || LineAy[i] != 0 && CroAy[i] != 0){
								if (((ax[maxAxx] - abAx) * (by[i] - ay[maxAxx]) + (ay[maxAxx] - abAy) * (ax[maxAxx] - bx[i]))
									* ((ax[maxAxx] - abAx) * (by[i + 1] - ay[maxAxx]) + (ay[maxAxx] - abAy) * (ax[maxAxx] - bx[i + 1])) < 0){
									if (((bx[i] - bx[i + 1]) * (ay[maxAxx] - by[i]) + (by[i] - by[i + 1]) * (bx[i] - ax[maxAxx]))
										* ((bx[i] - bx[i + 1]) * (abAy - by[i]) + (by[i] - by[i + 1]) * (bx[i] - abAx)) < 0){

										cdAx = LineAx[i] / CroAx[i];
										cdAy = LineAy[i] / CroAy[i];

										saAx = cdAx - ax[maxAxx];
										for (j = 0; j < AP; j++){
											ax[j] = ax[j] + saAx;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		//pieace1の重心計算
		gAx = 0; gAy = 0;
		for (i = 0; i < AP - 1; i++){
			gAx += ax[i];
			gAy += ay[i];
		}
		gAx = gAx / (AP - 1);
		gAy = gAy / (AP - 1);
		glColor3f(1.0, 0.0, 0.0);
		drawCircle(gAx, gAy, r);


		//peace1-正規化する前の座標保存
		for (i = 0; i < AP; i++){
			axx[i] = ax[i];
			ayy[i] = ay[i];
		}

		//peace1の正規化点 
		QminAx = ax[0];
		QminAy = ay[0];
		for (i = 1; i < AP-1; i++){
			if (QminAx > ax[i]){
				QminAx = ax[i];
			}
		}
		for (i = 1; i < AP-1; i++){
			if (QminAy > ay[i]){
				QminAy = ay[i];
			}
		}

		if (norm1 == AP){
			//正規化実行
			for (i = 0; i < AP; i++){
				ax[i] -= QminAx;
				ay[i] -= QminAy;
			}
			//pieace1(四角形)
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0, 1.0, 0.0);
			for (i = 0; i < AP; i++)
				glVertex2f(ax[i], ay[i]);
			glEnd();
		}

		//pieaece1のx座標比較（誤差計算用）
		Axtemp[0] = ax[0];
		Axtemp[1] = ax[0];
		for (i = 0; i < AP-1; i++){
			if (Axtemp[0] > ax[i])
				Axtemp[0] = ax[i];
			if (Axtemp[1] < ax[i])
				Axtemp[1] = ax[i];
		}
		//pieace1のy座標比較（誤差計算用）
		Aytemp[0] = ay[0];
		Aytemp[1] = ay[0];
		for (i = 0; i < AP-1; i++){
			if (Aytemp[0] > ay[i])
				Aytemp[0] = ay[i];
			if (Aytemp[1] < ay[i])
				Aytemp[1] = ay[i];
		}
		//2つの図形のx座標比較（誤差計算用）
		if (Qxtemp[0] < Axtemp[0])
			Aa = Qxtemp[0];
		else
			Aa = Axtemp[0];
		if (Qxtemp[1] > Axtemp[1])
			Ab = Qxtemp[1];
		else
			Ab = Axtemp[1];
		//2つの図形のy座標比較（誤差計算用）
		if (Qytemp[0] < Aytemp[0])
			Ac = Qytemp[0];
		else
			Ac = Aytemp[0];
		if (Qytemp[1] > Aytemp[1])
			Ad = Qytemp[1];
		else
			Ad = Aytemp[1];

		hA1 = round((Ab - Aa) / h) - 1;	//解像度数x
		hA2 = round((Ad - Ac) / h) - 1;	//解像度数y


		//pieace1の誤差計量
		for (i = 0; i <= hA1; i++){
			for (j = 0; j <= hA2; j++){
				hyoukaAx = 0.0001 + Aa + i * h; //評価点x
				hyoukaAy = 0.0001 + Ac + j * h; //評価点y
				drawCircle(hyoukaAx, hyoukaAy, r);	//評価範囲確認用

				//点の内外判定pieace1
				for (gi = 0; gi < AP - 1; gi++){
					if (((hyoukaAx - 5.0) * (ay[gi] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - ax[gi]))
						* ((hyoukaAx - 5.0) * (ay[gi + 1] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - ax[gi + 1])) < 0){
						if (((ax[gi] - ax[gi + 1]) * (hyoukaAy - ay[gi]) + (ay[gi] - ay[gi + 1]) * (ax[gi] - hyoukaAx))
							* ((ax[gi] - ax[gi - 1]) * (hyoukaAy - ay[gi]) + (ay[gi] - ay[gi + 1]) * (ax[gi] - 5.0)) < 0){
							An += 1;
						}
					}
				}
				if (An % 2 >= 1) { //pieace1図形内のとき
					//点の内外判定Q
					for (fi = 0; fi < QP - 1; fi++){
						if (((hyoukaAx - 5.0) * (Qy[fi] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - Qx[fi]))
							* ((hyoukaAx - 5.0) * (Qy[fi + 1] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - Qx[fi + 1])) < 0){
							if (((Qx[fi] - Qx[fi + 1]) * (hyoukaAy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - hyoukaAx))
								* ((Qx[fi] - Qx[fi - 1]) * (hyoukaAy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - 5.0)) < 0){
								QAn += 1;
							}
						}
					}
					if (QAn % 2 >= 1){
						Amatch += 1;
					}
					else{
						Aerror += 1;
					}
				}
				else{
					//点の内外判定Q
					for (fi = 0; fi < QP - 1; fi++){
						if (((hyoukaAx - 5.0) * (Qy[fi] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - Qx[fi]))
							* ((hyoukaAx - 5.0) * (Qy[fi + 1] - hyoukaAy) + (hyoukaAy - hyoukaAy) * (hyoukaAx - Qx[fi + 1])) < 0){
							if (((Qx[fi] - Qx[fi + 1]) * (hyoukaAy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - hyoukaAx))
								* ((Qx[fi] - Qx[fi - 1]) * (hyoukaAy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - 5.0)) < 0){
								QAn += 1;
							}
						}
					}
					if (QAn % 2 >= 1){	//Q図形内のとき
						Aerror += 1;
					}
				}
			}
		}
		Aratio = (double)Aerror / ((double)Amatch + (double)Aerror);
		printf("\nAmatch=%d\n", Amatch);
		printf("Aerror=%d\n", Aerror);
		printf("Aratio=%lf\n", Aratio);

		//正規化後の座標戻し
		for (i = 0; i < AP; i++){
			ax[i] = axx[i];
			ay[i] = ayy[i];
		} norm1 = 0;

		//ラベルの番号表示
		glColor3f(0.8, 0.0, 1.0);
		for (i = 0; i < AP - 1; i++){
			sprintf_s(str, "%d", i);
			render_stringA(ax[i], ay[i], str);
		}

		//pieace1(四角形)
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0);
		for (i = 0; i < AP - 1; i++){
			glVertex2f(ax[i], ay[i]);
			glVertex2f(ax[i + 1], ay[i + 1]);
		}
		glEnd();






		//pieace2角度
		thetaB = ran[1] * (3.14 / 180);

		//pieace2の距離計算
		for (i = 0; i < BP - 1; i++){
			disBx[i] = fabs(bx[0] - bx[i + 1]);
			disBy[i] = fabs(by[0] - by[i + 1]);
		}
		for (i = 0; i < BP - 1; i++){
			rrB[i] = sqrt(disBx[i] * disBx[i] + disBy[i] * disBy[i]);
		}

		//pieace2の角度計算
		if (iiii == 1){
			theta2[0] = atan(disBy[0] / disBx[0]);
			if (bx[0]>bx[1] && by[0]<by[1]) {  //左上にある
				theta2[0] = 3.14 - theta2[0];
			}
			if (bx[0]>bx[1] && by[0] > by[1]){ //左下にある
				theta2[0] += 3.14;
			}
			if (bx[0]<bx[1] && by[0]>by[1]) {	//右下にある
				theta2[0] = 2 * 3.14 - theta2[0];
			}

			for (i = 1; i < BP - 1; i++){
				theta2[i] = atan(disBy[i] / disBx[i]);
				if (bx[0]>bx[i + 1] && by[0]<by[i + 1]) {  //左上にある
					theta2[i] = 3.14 - theta2[i];
				}
				if (bx[0]>bx[i + 1] && by[0] > by[i + 1])  theta2[i] += 3.14;  //左下にある
				if (bx[0]<bx[i + 1] && by[0]>by[i + 1])  theta2[i] = 2 * 3.14 - theta2[i];  //右下にある
			}
		}
		else{
			theta2[0] = thetaBC;
			for (i = 1; i < BP - 2; i++){
				theta2[i] = thetaBD[i];
			}
		}

		thetaBC = theta2[0] + thetaB;
		if (thetaBC < 0)  thetaBC += 2 * 3.14;
		for (i = 1; i < BP - 1; i++){
			thetaBD[i] = theta2[i] + thetaB;
			if (thetaBD[i] < 0)  thetaBD[i] += 2 * 3.14;
		}

		//pieace2の角度動作
		bx[1] = bx[0] + rrB[0] * cos(thetaBC);
		by[1] = by[0] + rrB[0] * sin(thetaBC);
		for (i = 1; i < BP - 2; i++){
			bx[i + 1] = bx[0] + rrB[i] * cos(thetaBD[i]);
			by[i + 1] = by[0] + rrB[i] * sin(thetaBD[i]);
		}

		//pieace2の平行移動
		for (i = 0; i < BP; i++){
			if (ram[1] == 0) bx[i] += ran[3];
			else by[i] += ran[3];

			norm2 += 1; //正規化スイッチ
		}

		//pieace2の重心を求める
		for (i = 0; i < BP - 1; i++){
			gBx += bx[i];
			gBy += by[i];
		}
		gBx = gBx / (BP - 1);
		gBy = gBy / (BP - 1);

		//pieace2とpieace1の内外判定（近接用）
		//※図形の境界線上は図形外としている。これは境界線上が図形の大きさに含まれないためである。
		for (i = 0; i < AP - 1; i++){
			for (j = 0; j < BP - 1; j++){
				if (((bx[j] - 5.0) * (ay[i] - by[j]) + (by[j] - by[j]) * (bx[j] - ax[i]))
					* ((bx[j] - 5.0) * (ay[i + 1] - by[j]) + (by[j] - by[j]) * (bx[j] - ax[i + 1])) < 0){
					if (((ax[i] - ax[i + 1]) * (by[j] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[j]))
						* ((ax[i] - ax[i - 1]) * (by[j] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - 5.0)) < 0){
						gcn2[j] += 1;
					}
				}
			}
		}

		//pieace2-内部座標の個数確認（近接用）
		for (i = 0; i < BP; i++){
			if (gcn2[i] % 2 >= 1){
				gcng2 += 1;
			}
		}printf("近接動作の内部個数=%d\n", gcng2);

		//pieace2が離れている場合、四角形内部に入れる処理（近接）
		if (iiii>1){
			if (gcng2 == 0){
				//pieace2の距離計算
				for (i = 0; i < BP; i++){
					gdisBx[i] = -(gBx - bx[i]);
					gdisBy[i] = -(gBy - by[i]);
				}
				//pieace2の移動
				for (i = 0; i < BP; i++){
					gBx = gAx; gBy = gAy;
					bx[i] = gBx + gdisBx[i];
					by[i] = gBy + gdisBy[i];
				}
				ggg = 1;
				printf("重心まで移動%d\n", ggg);
			}
		}

		//pieace2とpieace1の内外判定（排出用）
		//※図形の境界線上は図形外としている。これは境界線上が図形の大きさに含まれないためである。
		for (i = 0; i < AP - 1; i++){
			for (j = 0; j < BP - 1; j++){
				if (((bx[j] - 5.0) * (ay[i] - by[j]) + (by[j] - by[j]) * (bx[j] - ax[i]))
					* ((bx[j] - 5.0) * (ay[i + 1] - by[j]) + (by[j] - by[j]) * (bx[j] - ax[i + 1])) < 0){
					if (((ax[i] - ax[i + 1]) * (by[j] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[j]))
						* ((ax[i] - ax[i - 1]) * (by[j] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - 5.0)) < 0){
						cn2[j] += 1;
					}
				}
			}
		}

		//pieace2-内部座標の個数確認（排出用）
		for (i = 0; i < BP -1 ; i++){
			if (cn2[i] % 2 >= 1){
				cnn2 += 1;
			}
		}printf("排出動作の内部個数=%d\n", cnn2);

		//pieace2が内部に入っている場合の計算
		minBy = 5.0;	maxBy = -5.0;
		minBx = 5.0;	maxBx = -5.0;
		for (i = 0; i < BP - 1; i++){
			if (cn2[i] % 2 >= 1){
				if (bx[i] > maxBx){ //内部のうち一番右の座標を特定
					maxBx = bx[i];
					maxBxx = i;
				}
				if (by[i] > maxBy){ //内部のうち一番上の座標を特定
					maxBy = by[i];
					maxByy = i;
				}
				if (bx[i] < minBx){ //内部のうち一番左の座標を特定
					minBx = bx[i];
					minBxx = i;
				}
				if (by[i] < minBy){ //内部のうち一番下の座標を特定
					minBy = by[i];
					minByy = i;
				}
			}
		}

		//pieace2-移動座標乱数
		nan2 = rand() % 4;

		//pieace2-図形内部の場合の移動処理1
		if (cnn2 == 1 || ggg == 0){//内部座標が一個の場合 
			printf("図形内部が一個\n");
			for (int ii = 0; ii < BP - 1; ii++){
				if (cn2[ii] % 2 >= 1){
					if (nan == 0){	//上に移動
						abBx = bx[ii];	abBy = 5.0;
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[ii] - abBx) * (ay[i] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i]))
									* ((bx[ii] - abBx) * (ay[i + 1] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[ii] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[ii]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBy = cdBy - by[ii];
										for (j = 0; j < BP; j++){
											by[j] = by[j] + saBy;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 1){	//右に移動
						abBx = 5.0;	abBy = by[ii];
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[ii] - abBx) * (ay[i] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i]))
									* ((bx[ii] - abBx) * (ay[i + 1] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[ii] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[ii]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBx = cdBx - bx[ii];
										for (j = 0; j < BP; j++){
											bx[j] = bx[j] + saBx;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 2){	//下に移動
						abBx = bx[ii];	abBy = -5.0;
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[ii] - abBx) * (ay[i] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i]))
									* ((bx[ii] - abBx) * (ay[i + 1] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[ii] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[ii]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBy = cdBy - by[ii];
										for (j = 0; j < BP; j++){
											by[j] = by[j] + saBy;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 3){	//左に移動
						abBx = -5.0;	abBy = by[ii];
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[ii])) - (((by[ii] * abBx) - (bx[ii] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[ii])*(ax[i] - ax[i + 1])) - ((abBx - bx[ii])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[ii] - abBx) * (ay[i] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i]))
									* ((bx[ii] - abBx) * (ay[i + 1] - by[ii]) + (by[ii] - abBy) * (bx[ii] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[ii] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[ii]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBx = cdBx - bx[ii];
										for (j = 0; j < BP; j++){
											bx[j] = bx[j] + saBx;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		//piace2-図形内部の場合の移動処理2
		else if (cnn2 >= 2 || ggg==1){//内部座標が複数個の場合
			printf("図形内部が複数\n");
			for (int ii = 0; ii < BP - 1; ii++){
				if (cn2[ii] % 2 >= 1){
					if (nan == 0){	//上に移動
						abBx = bx[minByy];	abBy = 5.0;

						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[minByy])) - (((by[minByy] * abBx) - (bx[minByy] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[minByy])*(ax[i] - ax[i + 1])) - ((abBx - bx[minByy])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[minByy])) - (((by[minByy] * abBx) - (bx[minByy] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[minByy])*(ax[i] - ax[i + 1])) - ((abBx - bx[minByy])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[minByy] - abBx) * (ay[i] - by[minByy]) + (by[minByy] - abBy) * (bx[minByy] - ax[i]))
									* ((bx[minByy] - abBx) * (ay[i + 1] - by[minByy]) + (by[minByy] - abBy) * (bx[minByy] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[minByy] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[minByy]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBy = cdBy - by[minByy];
										for (j = 0; j < BP; j++){
											by[j] = by[j] + saBy;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 1){	//右に移動
						abBx = 5.0;	abBy = by[minBxx];
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[minBxx])) - (((by[minBxx] * abBx) - (bx[minBxx] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[minBxx])*(ax[i] - ax[i + 1])) - ((abBx - bx[minBxx])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[minBxx])) - (((by[minBxx] * abBx) - (bx[minBxx] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[minBxx])*(ax[i] - ax[i + 1])) - ((abBx - bx[minBxx])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[minBxx] - abBx) * (ay[i] - by[minBxx]) + (by[minBxx] - abBy) * (bx[minBxx] - ax[i]))
									* ((bx[minBxx] - abBx) * (ay[i + 1] - by[minBxx]) + (by[minBxx] - abBy) * (bx[minBxx] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[minBxx] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[minBxx]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBx = cdBx - bx[minBxx];
										for (j = 0; j < BP; j++){
											bx[j] = bx[j] + saBx;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 2){	//下に移動
						abBx = bx[maxByy];	abBy = -5.0;
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[maxByy])) - (((by[maxByy] * abBx) - (bx[maxByy] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[maxByy])*(ax[i] - ax[i + 1])) - ((abBx - bx[maxByy])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[maxByy])) - (((by[maxByy] * abBx) - (bx[maxByy] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[maxByy])*(ax[i] - ax[i + 1])) - ((abBx - bx[maxByy])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[maxByy] - abBx) * (ay[i] - by[maxByy]) + (by[maxByy] - abBy) * (bx[maxByy] - ax[i]))
									* ((bx[maxByy] - abBx) * (ay[i + 1] - by[maxByy]) + (by[maxByy] - abBy) * (bx[maxByy] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[maxByy] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[maxByy]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBy = cdBy - by[maxByy];
										for (j = 0; j < BP; j++){
											by[j] = by[j] + saBy;
										}
									}
								}
							}
						}
					}
					else if (nan2 == 3){	//左に移動
						abBx = -5.0;	abBy = by[maxBxx];
						//線分交差判定
						for (i = 0; i < AP - 1; i++){
							LineBx[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBx - bx[maxBxx])) - (((by[maxBxx] * abBx) - (bx[maxBxx] * abBy))*(ax[i] - ax[i + 1])));
							CroBx[i] = (((abBy - by[maxBxx])*(ax[i] - ax[i + 1])) - ((abBx - bx[maxBxx])*(ay[i] - ay[i + 1])));
							LineBy[i] = ((((ay[i + 1] * ax[i]) - (ax[i + 1] * ay[i]))*(abBy - by[maxBxx])) - (((by[maxBxx] * abBx) - (bx[maxBxx] * abBy))*(ay[i] - ay[i + 1])));
							CroBy[i] = (((abBy - by[maxBxx])*(ax[i] - ax[i + 1])) - ((abBx - bx[maxBxx])*(ay[i] - ay[i + 1])));
						}
						for (i = 0; i < AP - 1; i++){
							if (LineBx[i] != 0 && CroBx[i] != 0 || LineBy[i] != 0 && CroBy[i] != 0){
								if (((bx[maxBxx] - abBx) * (ay[i] - by[maxBxx]) + (by[maxBxx] - abBy) * (bx[maxBxx] - ax[i]))
									* ((bx[maxBxx] - abBx) * (ay[i + 1] - by[maxBxx]) + (by[maxBxx] - abBy) * (bx[maxBxx] - ax[i + 1])) < 0){
									if (((ax[i] - ax[i + 1]) * (by[maxBxx] - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - bx[maxBxx]))
										* ((ax[i] - ax[i + 1]) * (abBy - ay[i]) + (ay[i] - ay[i + 1]) * (ax[i] - abBx)) < 0){

										cdBx = LineBx[i] / CroBx[i];
										cdBy = LineBy[i] / CroBy[i];

										saBx = cdBx - bx[maxBxx];
										for (j = 0; j < BP; j++){
											bx[j] = bx[j] + saBx;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		//pieace2の重心確認用
		gBx = 0; gBy = 0;
		for (i = 0; i < BP - 1; i++){
			gBx += bx[i];
			gBy += by[i];
		}
		gBx = gBx / (BP - 1);
		gBy = gBy / (BP - 1);
		glColor3f(0.0, 0.0, 1.0);
		drawCircle(gBx, gBy, r);


		//peace2-正規化のための座標保存
		for (i = 0; i < BP; i++){
			bxx[i] = bx[i];
			byy[i] = by[i];
		}

		//peace1の正規化点 
		QminBx = bx[0];
		QminBy = by[0];
		for (i = 1; i < BP - 1; i++){
			if (QminBx > bx[i]){
				QminBx = bx[i];
			}
		}
		for (i = 1; i < BP - 1; i++){
			if (QminBy > by[i]){
				QminBy = by[i];
			}
		}

		if (norm2 == BP){
			//正規化実行
			for (i = 0; i < BP; i++){
				bx[i] -= QminAx;
				by[i] -= QminAy;
			}
			//pieace2(三角形)
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0, 1.0, 0.0);
			for (i = 0; i < BP; i++)
				glVertex2f(bx[i], by[i]);
			glEnd();

		}


		//pieace2のx座標比較（誤差計算用）
		Bxtemp[0] = bx[0];
		Bxtemp[1] = bx[0];
		for (i = 0; i < BP - 1; i++){
			if (Bxtemp[0] > bx[i])
				Bxtemp[0] = bx[i];
			if (Bxtemp[1] < bx[i])
				Bxtemp[1] = bx[i];
		}
		//pieace2のy座標比較（誤差計算用）
		Bytemp[0] = by[0];
		Bytemp[1] = by[0];
		for (i = 0; i < BP - 1; i++){
			if (Bytemp[0] > by[i])
				Bytemp[0] = by[i];
			if (Bytemp[1] < by[i])
				Bytemp[1] = by[i];
		}
		//2つの図形のx座標比較（誤差計算用）
		if (Qxtemp[0] < Bxtemp[0])
			Ba = Qxtemp[0];
		else
			Ba = Bxtemp[0];
		if (Qxtemp[1] > Bxtemp[1])
			Bb = Qxtemp[1];
		else
			Bb = Bxtemp[1];
		//2つの図形のy座標比較（誤差計算用）
		if (Qytemp[0] < Bytemp[0])
			Bc = Qytemp[0];
		else
			Bc = Bytemp[0];
		if (Qytemp[1] > Bytemp[1])
			Bd = Qytemp[1];
		else
			Bd = Bytemp[1];

		hB1 = round((Bb - Ba) / h) - 1;	//解像度数x
		hB2 = round((Bd - Bc) / h) - 1;	//解像度数y

		//pieace2の誤差計量
		for (i = 0; i <= hB1; i++){
			for (j = 0; j <= hB2; j++){
				hyoukaBx = 0.0001 + Ba + i * h; //評価点x
				hyoukaBy = 0.0001 + Bc + j * h; //評価点y
				//drawCircle(hyoukaBx, hyoukaBy, r);	//評価範囲確認用
				//printf("(%d,%d)\n", i, j);

				if (fabs(bx[i] - bx[i + 1]) < 0.0001){
					bx[i + 1] += 0.00001;
				}
				double af = (by[i] - by[i + 1]) / (bx[i] - bx[i + 1]);
				double bf = by[i] - af*bx[i];
				double yf = af* hyoukaBx + bf;
				double xf = (yf - bf) / af;
				if (fabs(hyoukaBx - xf) < 0.0001)
					continue;
				else if (i == 1 && j == 0)
					printf("%lf\n", (fabs(hyoukaBx - xf)));
				if (fabs(hyoukaBy - yf) < 0.0001)
					continue;
				else if (i == 1 && j == 0)
					printf("%lf\n", (fabs(hyoukaBy - yf)));

				//点の内外判定pieace2
				for (gi = 0; gi < BP - 1; gi++){					
					if (((hyoukaBx - 5.0) * (by[gi] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - bx[gi]))
						* ((hyoukaBx - 5.0) * (by[gi + 1] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - bx[gi + 1])) < 0){
						if (((bx[gi] - bx[gi + 1]) * (hyoukaBy - by[gi]) + (by[gi] - by[gi + 1]) * (bx[gi] - hyoukaBx))
							* ((bx[gi] - bx[gi - 1]) * (hyoukaBy - by[gi]) + (by[gi] - by[gi + 1]) * (bx[gi] - 5.0)) < 0){

							Bn += 1;
						}
					}
				}
				if (Bn % 2 >= 1) { //pieace2図形内のとき
					//点の内外判定piece2
					for (fi = 0; fi < QP - 1; fi++){
						if (((hyoukaBx - 5.0) * (Qy[fi] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - Qx[fi]))
							* ((hyoukaBx - 5.0) * (Qy[fi + 1] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - Qx[fi + 1])) < 0){
							if (((Qx[fi] - Qx[fi + 1]) * (hyoukaBy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - hyoukaBx))
								* ((Qx[fi] - Qx[fi - 1]) * (hyoukaBy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - 5.0)) < 0){

								QBn += 1;
							}
						}
					}
					if (QBn % 2 >= 1){	//Q図形内のとき
						Bmatch += 1;
						//printf("m\n");
					}
					else {
						Berror += 1;
						//printf("e\n");
					}
				}
				else{
					if (fabs(Qx[i] - Qx[i + 1]) < 0.0001){
						Qx[i + 1] += 0.00001;
					}
					double ag = (Qy[i] - Qy[i + 1]) / (Qx[i] - Qx[i + 1]);
					double bg = Qy[i] - ag*Qx[i];
					double yg = ag* hyoukaBx + bg;
					double xg = (yg - bg) / ag;
					if (fabs(hyoukaBx - xg) < 0.0001)
						continue;
					else if (i == 1 && j == 0)
						printf("%lf\n", (fabs(hyoukaBx - xg)));
					if (fabs(hyoukaBy - yg) < 0.0001)
						continue;
					else if (i == 1 && j == 0)
						printf("%lf\n", (fabs(hyoukaBy - yg)));

					//点の内外判定Q
					for (fi = 0; fi < QP - 1; fi++){
						if (((hyoukaBx - 5.0) * (Qy[fi] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - Qx[fi]))
							* ((hyoukaBx - 5.0) * (Qy[fi + 1] - hyoukaBy) + (hyoukaBy - hyoukaBy) * (hyoukaBx - Qx[fi + 1])) < 0){
							if (((Qx[fi] - Qx[fi + 1]) * (hyoukaBy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - hyoukaBx))
								* ((Qx[fi] - Qx[fi - 1]) * (hyoukaBy - Qy[fi]) + (Qy[fi] - Qy[fi + 1]) * (Qx[fi] - 5.0)) < 0){

								QBn += 1;
							}
						}
					}
					if (QBn % 2 >= 1){	//Q図形内のとき
						Berror += 1;
						//printf("e\n");
					}
				}
			}
		}
		Bratio = (double)Berror / ((double)Bmatch + (double)Berror);
		printf("Bmatch=%d\n", Bmatch);
		printf("Berror=%d\n", Berror);
		printf("Bratio=%lf\n", Bratio);




		//正規化後の座標戻し
		for (i = 0; i < BP; i++){
			bx[i] = bxx[i];
			by[i] = byy[i];
		} norm2 = 0;

		//ラベルの番号表示
		glColor3f(0.0, 0.0, 1.0);
		for (i = 0; i < BP - 1; i++){
			sprintf_s(str, "%d", i);
			render_stringA(bx[i], by[i], str);
		}

		//pieace2(三角形)
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0);
		for (i = 0; i < BP - 1; i++){
			glVertex2f(bx[i], by[i]);
			glVertex2f(bx[i + 1], by[i + 1]);
		}
		glEnd();

		glFlush();

		//角度乱数	※図形が離れていくのはSAで調整
		ran[0] = (double)(rand() % 21 - 10);
		ran[1] = (double)(rand() % 21 - 10);
		//平行移動乱数
		ran[2] = (double)(rand() % 2) / 10;
		ran[3] = (double)(rand() % 2) / 10;
		if (ran[2] == 0) ran[2] -= 0.1;	//0の時は-0.1にする
		if (ran[3] == 0) ran[3] -= 0.1;	//0の時は-0.1にする
		//平行移動スイッチ
		ram[0] = (double)(rand() % 2);
		ram[1] = (double)(rand() % 2);

		//内外判定の初期化
		for (i = 0; i < AP - 1; i++){
			cn1[i] = 0;
		}
		nan1 = 0; cnn1 = 0;
		for (i = 0; i < BP - 1; i++){
			cn2[i] = 0;
		}
		nan2 = 0; cnn2 = 0;

		//近接処理の初期化
		for (i = 0; i < BP - 1; i++){
			gcn2[i] = 0;
		}
		gcng2 = 0;

		//内部三角形の最大最小の初期化
		maxAxx = 0; minAxx = 0;
		maxAyy = 0; minAyy = 0;
		maxBxx = 0; minBxx = 0;
		maxByy = 0; minByy = 0;

		//重心の初期化
		gAx = 0; gAy = 0; gBx = 0; gBy = 0;
		ggg = 0;

		//誤差判定の初期化
		QAn = 0; QBn = 0; An = 0; Bn = 0;
		Aerror = 0; Amatch = 0; Berror = 0; Bmatch = 0;

		getchar();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(400, 400);  // ウィンドウサイズは400x400ドット
	glutInitWindowPosition(600, 300);  // ウィンドウサイズの表示位置の指定
	glutCreateWindow("simple");
	glOrtho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0);

	printf("開始します。何かキーを押してください。");  getchar();

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}