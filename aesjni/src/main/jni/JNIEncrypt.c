#include <jni.h>
#include "aes.h"
#include <string.h>
#include "signaturecheck.h"
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define CBC 1
#define ECB 1

const char *UNSIGNATURE = "UNSIGNATURE";

JNIEXPORT jstring JNICALL
Java_com_androidyuan_aesjni_AESEncrypt_encode(JNIEnv *env, jobject instance, jobject context, jstring str_) {

    //先进行apk被 二次打包的校验
    if (checkSignature(env, instance, context) != 1) {
        char *str = UNSIGNATURE;
        return (*env)->NewString(env, str, strlen(str));
    }

    const char *in = (*env)->GetStringUTFChars(env, str_, JNI_FALSE);
    char *baseResult = AES_128_ECB_PKCS5Padding_Encrypt(in, AES_KEY);
    (*env)->ReleaseStringUTFChars(env, str_, in);
    return (*env)->NewStringUTF(env, baseResult);
}

JNIEXPORT jstring JNICALL
Java_com_androidyuan_aesjni_AESEncrypt_decode(JNIEnv *env, jobject instance, jobject context, jstring str_) {


    //先进行apk被 二次打包的校验
    if (checkSignature(env, instance, context) != 1) {
        char *str = UNSIGNATURE;
        return (*env)->NewString(env, str, strlen(str));
    }


    const char *str = (*env)->GetStringUTFChars(env, str_, JNI_FALSE);
    char *desResult = AES_128_ECB_PKCS5Padding_Decrypt(str, AES_KEY);
    (*env)->ReleaseStringUTFChars(env, str_, str);
    return (*env)->NewStringUTF(env, desResult);
    //不用系统自带的方法NewStringUTF是因为如果desResult是乱码,会抛出异常
    //return charToJstring(env,desResult);
}


jstring charToJstring(JNIEnv *envPtr, char *src) {
    JNIEnv env = *envPtr;

    jsize len = strlen(src);
    jclass clsstring = env->FindClass(envPtr, "java/lang/String");
    jstring strencode = env->NewStringUTF(envPtr, "UTF-8");
    jmethodID mid = env->GetMethodID(envPtr, clsstring, "<init>",
                                     "([BLjava/lang/String;)V");
    jbyteArray barr = env->NewByteArray(envPtr, len);
    env->SetByteArrayRegion(envPtr, barr, 0, len, (jbyte *) src);

    return (jstring) env->NewObject(envPtr, clsstring, mid, barr, strencode);
}


/**
 * if rerurn 1 ,is check pass.
 */
JNIEXPORT jint JNICALL
Java_com_androidyuan_aesjni_AESEncrypt_checkSignature(JNIEnv *env, jobject instance, jobject con) {

    return checkSignature(env, instance, con);
}

