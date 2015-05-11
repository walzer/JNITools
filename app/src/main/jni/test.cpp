
#include "javaenv.h"
#include "JavaDispatcher.h"
#include "NativeBridge.h"
#include "Proxy.h"
#include "JNIUtils.h"

using namespace sdkbox;

class GA  {
public:

   void A( const std::string& event, jobject params ) {

        JNIEnv *env= __getJNIEnv();

        std::string rec= JNIUtils::NewStringFromJString( env, (jstring)params );

        LOGD("GA callback event:%s recv:%s", event.c_str(), rec.c_str() );
   }

};

extern "C" {

    void JNICALL test3(JNIEnv* env, jobject thiz) {

        SPProxy p= Proxy::New( 
            "org/cocos2dx/example/TestProxy",
            JNIArray::NewFromCharPtrArrayV( env, "jaja", "jeje", NULL )->get() );

        // String m1();
        jobject obj= p->invoke("m1");
            LOGD("Result from proxy m1: %s", JNIUtils::NewStringFromJString( env, (jstring)obj ).c_str());
            env->DeleteLocalRef( obj );

        // void m2()
        obj= p->invoke("m2");

        obj= p->invoke("m3", JNIUtils::NewArray(env, 2)->addString(env, "str").addInt(env, 23).get() );
//            LOGD("Result from proxy m3: %d", JNIUtils.getIntValue(obj) );
            env->DeleteLocalRef( obj );
    }

    void JNICALL test2(JNIEnv* env, jobject thiz) {

        SPCallback* gg= new SPCallback( new JNIEventListener<GA>( new GA(), &GA::A ) );
        // BUGBUG leaking
        NativeBridge::addEventListener( "GoogleAnalytics.Tick", gg );

    }

    void JNICALL test1(JNIEnv* env, jobject thiz) {

        JavaDispatcher::callInService(
                "org/cocos2dx/services/GoogleAnalytics",
                "logScreen",
                JNIArray::NewFromCharPtrArrayV( env, "hellou", NULL )->get() );


        JavaDispatcher::callInService(
                "org/cocos2dx/services/GoogleAnalytics",
                "logScreen2",
                JNIArray::NewFromCharPtrArrayV( env, "hellou", "3-405683-083530-84", NULL )->get() );

        JavaDispatcher::callInService(
                "org/cocos2dx/services/GoogleAnalytics",
                "logScreen3",
                JNIUtils::NewArray( env, 3 )->addString(env,"str0").
                            addString(env,"str1").
                            addInt(env,5).
                            get() );

        JavaDispatcher::callStatic(
            "org/cocos2dx/csc/CSC",
            "test_call",
            JNIArray::NewFromCharPtrArrayV( env, "calling", "CSC::test_call static method.", NULL )->get() );

    }

    JNIEXPORT
    void JNICALL Java_org_cocos2dx_csc_CSC_testSendMessages(JNIEnv* env, jobject thiz) {
        test1( env, thiz );
        test2( env, thiz );
        test3( env, thiz );
    }

}