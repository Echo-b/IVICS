package com.example.CrossingInfoServer.Service;

import com.example.CrossingInfoServer.Entity.CrossingInfo;
import com.example.CrossingInfoServer.Entity.CrossingInfoData;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 15:35
 */
@Service
public class CrossingInfoService {
    @Autowired
    CrossingInfoData data;

    public void setCrossingInfo(CrossingInfo info){
        data.myUpdate(info);
    }
    public CrossingInfo getCrossingInfo(){
        return data.getInfo();
    }
}
