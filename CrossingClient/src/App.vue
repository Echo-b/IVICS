<template>
  <div id="app">
    <h3 align="left">道口信息</h3>
    <el-descriptions :column="1" border>
      <el-descriptions-item label="地址">{{
        crossingInfo.address
      }}</el-descriptions-item>
      <el-descriptions-item label="时间">{{
        crossingInfo.time
      }}</el-descriptions-item>
      <el-descriptions-item label="道口状态">{{
        crossingInfo.crossingStatus
      }}</el-descriptions-item>
      <el-descriptions-item label="红绿灯状态">{{
        crossingInfo.ledStatus
      }}</el-descriptions-item>
      <el-descriptions-item label="栏杆状态">{{
        crossingInfo.barrierStatus
      }}</el-descriptions-item>
      <el-descriptions-item label="车辆数量">{{
        crossingInfo.vehicleNum
      }}</el-descriptions-item>
    </el-descriptions>
    <h3 align="left">车辆信息</h3>
    <el-table :data="crossingInfo.vehicleInflows" style="width: 100%">
      <el-table-column prop="id" label="id" width="180"> </el-table-column>
      <el-table-column prop="kind" label="类型" width="180"> </el-table-column>
      <el-table-column prop="speed" label="速度"> </el-table-column>
    </el-table>
  </div>
</template>

<script>
export default {
  name: "App",
  components: {},

  data() {
    return {
      crossingInfo: {},
      tableData: [
        {
          date: "2016-05-02",
          name: "王小虎",
          address: "上海市普陀区金沙江路 1518 弄",
        },
        {
          date: "2016-05-04",
          name: "王小虎",
          address: "上海市普陀区金沙江路 1517 弄",
        },
        {
          date: "2016-05-01",
          name: "王小虎",
          address: "上海市普陀区金沙江路 1519 弄",
        },
        {
          date: "2016-05-03",
          name: "王小虎",
          address: "上海市普陀区金沙江路 1516 弄",
        },
      ],
    };
  },
  created() {
    this.initWebSocket();
  },
  methods: {
    // currentTime() {
    // 	setInterval(this.formatDate, 500);
    // },
    initWebSocket() {
      //初始化weosocket
      const wsuri = "ws://192.168.1.109:8080/crossing";
      this.websock = new WebSocket(wsuri);
      // 客户端接收服务端数据时触发
      this.websock.onmessage = this.websocketonmessage;
      // 连接建立时触发
      this.websock.onopen = this.websocketonopen;
      // 通信发生错误时触发
      this.websock.onerror = this.websocketonerror;
      // 连接关闭时触发
      this.websock.onclose = this.websocketclose;
    },
    // 连接建立时触发
    websocketonopen() {
      //开启心跳
      //this.start();
      //连接建立之后执行send方法发送数据
      // let actions = {"room":"007854ce7b93476487c7ca8826d17eba","info":"1121212"};
      // this.websocketsend(JSON.stringify(actions));
      console.log("连接成功");
    },
    // 通信发生错误时触发
    websocketonerror() {
      console.log("出现错误");
      //this.reconnect();
    },
    // 客户端接收服务端数据时触发
    websocketonmessage(e) {
      console.log(e.data);
      console.log(JSON.parse(e.data));
      this.crossingInfo = JSON.parse(e.data);
      //收到服务器信息，心跳重置
      //this.reset();
    },
    websocketsend(Data) {
      //数据发送
      this.websock.send(Data);
    },
    // 连接关闭时触发
    websocketclose(e) {
      //关闭
      console.log("断开连接", e);
      //重连
    },
  },
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
