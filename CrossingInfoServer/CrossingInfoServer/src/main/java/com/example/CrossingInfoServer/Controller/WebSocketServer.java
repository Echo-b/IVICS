package com.example.CrossingInfoServer.Controller;

import com.alibaba.fastjson.JSON;
import com.example.CrossingInfoServer.Entity.CrossingInfoData;
import com.example.CrossingInfoServer.Service.CrossingInfoService;
import jakarta.websocket.OnClose;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.concurrent.CopyOnWriteArraySet;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 21:38
 */
@Component
@Slf4j
@ServerEndpoint("/crossing")
public class WebSocketServer {
    /**
     * 线程安全的无序的集合
     */
    private static final CopyOnWriteArraySet<Session> SESSIONS = new CopyOnWriteArraySet<>();


    @OnOpen
    public void onOpen(Session session) {
        try {
            SESSIONS.add(session);
            log.info("【WebSocket消息】有新的连接，总数为：" + SESSIONS.size());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @OnClose
    public void onClose(Session session) {
        try {
            SESSIONS.remove(session);
            log.info("【WebSocket消息】连接断开，总数为：" + SESSIONS.size());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @OnMessage
    public void onMessage(String message) {
        log.info("【WebSocket消息】收到客户端消息：" + message);
    }

    /**
     * 此为广播消息
     *
     * @param message 消息
     */
    public static void sendAllMessage(String message) {
        log.info("【WebSocket消息】广播消息：" + message);
        for (Session session : SESSIONS) {
            try {
                if (session.isOpen()) {
                    session.getAsyncRemote().sendText(message);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

}
