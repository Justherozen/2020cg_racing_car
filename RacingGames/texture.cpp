#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include"gl/glut.h"
#include "texture.h"
#define BITMAP_ID 0x3D42
//������ͼƬ  
unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;    // �ļ�ָ��  
	BITMAPFILEHEADER bitmapFileHeader;    // bitmap�ļ�ͷ  
	unsigned char    *bitmapImage;        // bitmapͼ������  
	int    imageIdx = 0;        // ͼ��λ������  
	unsigned char    tempRGB;    // ��������  

								// �ԡ�������+����ģʽ���ļ�filename   
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) {
		printf("file not open\n");
		return NULL;
	}
	// ����bitmap�ļ�ͼ  
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// ��֤�Ƿ�Ϊbitmap�ļ�  
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}
	// ����bitmap��Ϣͷ  
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// ���ļ�ָ������bitmap����  
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// Ϊװ��ͼ�����ݴ����㹻���ڴ�  
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// ��֤�ڴ��Ƿ񴴽��ɹ�  
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// ����bitmapͼ������  
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// ȷ�϶���ɹ�  
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}
	//����bitmap�б���ĸ�ʽ��BGR�����潻��R��B��ֵ���õ�RGB��ʽ  
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// �ر�bitmapͼ���ļ�  
	fclose(filePtr);
	return bitmapImage;
}

GLuint LoadTexture(const string filename) {
	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap��Ϣͷ  
	unsigned char*   bitmapData;                                       // ��������
	const char* fp = filename.c_str();
	bitmapData = LoadBitmapFile(fp, &bitmapInfoHeader);
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLenum image_format = GL_RGB;
	GLint internal_format = GL_BGRA_EXT;

	glTexImage2D(GL_TEXTURE_2D, 0, image_format, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, internal_format, GL_UNSIGNED_BYTE, bitmapData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return id;
}


