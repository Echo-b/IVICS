package com.example.CrossingInfoServer.Entity;

import lombok.Data;
import org.springframework.stereotype.Component;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 13:37
 */
@Data
@Component
public class VehicleInfo {
    private int id;
    private String kind;
    private int speed;
}
