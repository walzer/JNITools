package org.cocos2dx.services;

import android.util.Log;

import org.cocos2dx.csc.Service;
import org.cocos2dx.csc.ServicesRegistry;
import org.json.JSONObject;

/**
 * Created by ibon on 5/8/15.
 */
public class AbstractService implements Service {

    protected ServicesRegistry    _registry;

    AbstractService() {

    }

    public void setRegistry( ServicesRegistry sr ) {
        _registry= sr;
    }

    @Override
    public void onPause() {
        Log.d("AbstractService", "onPause");
    }

    @Override
    public void onResume() {
        Log.d("AbstractService", "onResume");
    }

    @Override
    public void onDestroy() {
        Log.d("AbstractService", "onDestroy");
    }

    @Override
    public void onTrimMemory(int level) {
        Log.d("AbstractService", "onTrimMemory "+level);
    }

    @Override
    public boolean initialize(JSONObject initData) {
        return false;
    }

    @Override
    public String getName() {
        return null;
    }

    public void emit( String event, Object params ) {
        _registry.nativeEmit( event, params );
    }
}
