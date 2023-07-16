package com.example.CrossingInfoServer.Entity;

import lombok.Data;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;

import java.util.List;

/**
 * @author bye
 * @version 1.0
 * @create 2023-04-26 13:34
 */
@Data
@Component
public class CrossingInfo {
    private String crossingStatus;
    private String ledStatus;
    private String barrierStatus;
    private int vehicleNum;
    private String time;
    private String address;
    private List<VehicleInfo> vehicleInflows;
}
