#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "Header.h"
#define BUFFSIZE 2048
#define DELIMITER "."
//ディレクトリ取得
void getGurrentDirectory(char* currentDirectory) {
    GetCurrentDirectory(BUFFSIZE, currentDirectory);
}

//キーワードと拡張子が条件に合うファイルを検索し、結果をresultに保存
void findFile(char* dir, Strings* keyword, Strings* keyExtension, Strings* result);

//フォルダーでなければ分析する。不審なファイルである場合１を返す
int potentialTarget(char* fileName, Strings* keyword, Strings* extension);

int main()
{
    char currentDirectory[BUFFSIZE];
    getGurrentDirectory(currentDirectory);
    char settingFile[BUFFSIZE];
    //設定ファイルから子供と自分の言い方をロード
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
    //文字列の列であるStrings型でデータを保存する
    Strings* keyword = new Strings();
    Strings* keyExtension = new Strings();
    Strings* result = new Strings();

    //ファイルからキーワードと拡張子を読み込む
    FILE* fp;
    errno_t error = fopen_s(&fp, "keyword.txt", "r");
    if (error) {
        fprintf(stdout, "Failed to load keyword");
        return 0;
    }
    char buffer[BUFFSIZE];
    //行末の改行コードを削除
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
    //探索範囲
    char targetDir[BUFFSIZE];
    sprintf_s(targetDir, "%s\\test", currentDirectory);
    //探索
    findFile(targetDir, keyword, keyExtension, result);
    //ファイルの誤削除を防ぐために、削除する前に確認
    char key;
    for (int i = 0; i < (*result).size; i++) {
        fprintf(stdout, "ファイル");
        fprintf(stdout, (*result).strings[i]);
        fprintf(stdout, "には不健全な内容が含まれる可能性がある。削除しますか(Y/N)\n");
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
                sprintf_s(newFileDir, "%s\\%sへ.txt", tempstr, kid);
                error = fopen_s(&fp, newFileDir, "w");
                if (error) {
                    fprintf(stdout, "Failed to create message");
                    return 0;
                }
                //子供にノートを残してあげる
                fprintf(fp, "%sは%sが隠している不健全な動画を削除してあげました。 -- %s", self, kid, self);
                fclose(fp);
                break;
            }
            else if (key == 'n'||key=='N') {
                break;
            }
        }

    }
    fprintf(stdout, "おめでとうございます！%sのパソコンから不健全な動画をすべて削除しました。これから%sは純粋な子供として育てるようになりました。", kid, kid);
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