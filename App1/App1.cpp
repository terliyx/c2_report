#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "Header.h"
#define BUFFSIZE 2048
#define DELIMITER "."
//�f�B���N�g���擾
void getGurrentDirectory(char* currentDirectory) {
    GetCurrentDirectory(BUFFSIZE, currentDirectory);
}

//�L�[���[�h�Ɗg���q�������ɍ����t�@�C�����������A���ʂ�result�ɕۑ�
void findFile(char* dir, Strings* keyword, Strings* keyExtension, Strings* result);

//�t�H���_�[�łȂ���Ε��͂���B�s�R�ȃt�@�C���ł���ꍇ�P��Ԃ�
int potentialTarget(char* fileName, Strings* keyword, Strings* extension);

int main()
{
    char currentDirectory[BUFFSIZE];
    getGurrentDirectory(currentDirectory);
    char settingFile[BUFFSIZE];
    //�ݒ�t�@�C������q���Ǝ����̌����������[�h
    sprintf_s(settingFile, "%s\\setting.ini", currentDirectory);
    char kid[BUFFSIZE];
    if (GetPrivateProfileString("s1", "kid", "", kid, BUFFSIZE, settingFile) == 0) {
        fprintf(stdout, "Failed to load setting");
        return 0;
    }
    char self[BUFFSIZE];
    if (GetPrivateProfileString("s1", "self", "", self, BUFFSIZE, settingFile) == 0) {
        fprintf(stdout, "Failed to load setting");
        return 0;
    }
    //������̗�ł���Strings�^�Ńf�[�^��ۑ�����
    Strings* keyword = new Strings();
    Strings* keyExtension = new Strings();
    Strings* result = new Strings();

    //�t�@�C������L�[���[�h�Ɗg���q��ǂݍ���
    FILE* fp;
    errno_t error = fopen_s(&fp, "keyword.txt", "r");
    if (error) {
        fprintf(stdout, "Failed to load keyword");
        return 0;
    }
    char buffer[BUFFSIZE];
    //�s���̉��s�R�[�h���폜
    while (fgets(buffer, BUFFSIZE, fp)) {
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        strcpy_s((*keyword).strings[(*keyword).size], BUFFSIZE, buffer);
        (*keyword).size++;
    }
    fclose(fp);
    error = fopen_s(&fp, "keyExtension.txt", "r");
    if (error) {
        fprintf(stdout, "Failed to load keyExtension");
        return 0;
    }
    while (fgets(buffer, BUFFSIZE, fp)) {
        if (buffer[strlen(buffer)-1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        strcpy_s((*keyExtension).strings[(*keyExtension).size],BUFFSIZE, buffer);
        (*keyExtension).size++;
    }
    fclose(fp);
    //�T���͈�
    char targetDir[BUFFSIZE];
    sprintf_s(targetDir, "%s\\test", currentDirectory);
    //�T��
    findFile(targetDir, keyword, keyExtension, result);
    //�t�@�C���̌�폜��h�����߂ɁA�폜����O�Ɋm�F
    char key;
    for (int i = 0; i < (*result).size; i++) {
        fprintf(stdout, "�t�@�C��");
        fprintf(stdout, (*result).strings[i]);
        fprintf(stdout, "�ɂ͕s���S�ȓ��e���܂܂��\��������B�폜���܂���(Y/N)\n");
        while (1) {
            scanf_s("%c", &key);
            if (key == 'y'||key=='Y') {
                DeleteFile((*result).strings[i]);
                int j = strlen((*result).strings[i]) - 1;
                char tempstr[BUFFSIZE];
                strcpy_s(tempstr, BUFFSIZE, (*result).strings[i]);
                char c = tempstr[j];
                while (c != '\\') {
                    tempstr[j] = '\0';
                    j--;
                    c = tempstr[j];
                }
                char newFileDir[BUFFSIZE];
                sprintf_s(newFileDir, "%s\\%s��.txt", tempstr, kid);
                error = fopen_s(&fp, newFileDir, "w");
                if (error) {
                    fprintf(stdout, "Failed to create message");
                    return 0;
                }
                //�q���Ƀm�[�g���c���Ă�����
                fprintf(fp, "%s��%s���B���Ă���s���S�ȓ�����폜���Ă����܂����B -- %s", self, kid, self);
                fclose(fp);
                break;
            }
            else if (key == 'n'||key=='N') {
                break;
            }
        }

    }
    fprintf(stdout, "���߂łƂ��������܂��I%s�̃p�\�R������s���S�ȓ�������ׂč폜���܂����B���ꂩ��%s�͏����Ȏq���Ƃ��Ĉ�Ă�悤�ɂȂ�܂����B", kid, kid);
    while (1);

}

void findFile(char* dir, Strings* keyword, Strings* keyExtension, Strings* result) {
    char directory[BUFFSIZE];
    sprintf_s(directory, "%s\\*", dir);
    HANDLE hf;
    WIN32_FIND_DATA wfd;
    hf = FindFirstFile(directory, &wfd);
    do {
        if (strcmp(wfd.cFileName, ".") * strcmp(wfd.cFileName, "..")==0) {
            continue;
        }
        else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char newDir[BUFFSIZE];
            sprintf_s(newDir, "%s\\%s", dir, wfd.cFileName);
            findFile(newDir, keyword, keyExtension, result);
        }
        else {
            if (potentialTarget(wfd.cFileName, keyword, keyExtension)) {
                char fullDir[BUFFSIZE];
                sprintf_s(fullDir, "%s\\%s", dir, wfd.cFileName);
                strcpy_s((*result).strings[(*result).size], BUFFSIZE, fullDir);
                (*result).size++;
            }
        }
    } while (FindNextFile(hf, &wfd));

}

int potentialTarget(char* fileName, Strings* keyword, Strings* keyExtension) {
    char* name;
    char* extension;
    char* buffer;
    char tempstr[BUFFSIZE];
    strcpy_s(tempstr, BUFFSIZE, fileName);
    name = strtok_s(tempstr, DELIMITER, &buffer);
    extension = strtok_s(NULL, DELIMITER, &buffer);
    for (int i = 0; i < (*keyExtension).size; i++) {
        if (strcmp(extension, (*keyExtension).strings[i])==0) {
            for (int j = 0; j < (*keyword).size; j++) {
                if (strstr(name, (*keyword).strings[j])!=NULL) {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}