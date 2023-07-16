package com.example.CrossingInfoServer.Entity;

import com.alibaba.fastjson.JSON;
import com.example.CrossingInfoServer.Controller.WebSocketServer;
import lombok.Data;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.boot.web.server.WebServer;
import org.springframework.stereotype.Component;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 15:37
 */
@Data
@Component
public class CrossingInfoData {
    CrossingInfo info;

    public void myUpdate(CrossingInfo crossingInfo){
        info = crossingInfo;
        myNotify();
    }

    private void myNotify(){
        System.out.println(JSON.toJSONString(this.info));
        WebSocketServer.sendAllMessage(JSON.toJSONString(this.info));
    }

}
