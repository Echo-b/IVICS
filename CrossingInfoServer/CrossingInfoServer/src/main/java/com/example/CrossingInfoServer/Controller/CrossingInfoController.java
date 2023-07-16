package com.example.CrossingInfoServer.Controller;

import com.alibaba.fastjson.JSON;
import com.example.CrossingInfoServer.Entity.CrossingInfo;
import com.example.CrossingInfoServer.Service.CrossingInfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 15:41
 */
@RestController
public class CrossingInfoController {
    @Autowired
    CrossingInfoService service;

    @PostMapping(value = "/crossing")
    public void setCrossingInfo(@RequestBody String info){
        System.out.println(info);
        CrossingInfo data = JSON.parseObject(info,CrossingInfo.class);
        System.out.println(data.toString());
        service.setCrossingInfo(data);
        System.out.println(service.getCrossingInfo().toString());
    }

}
