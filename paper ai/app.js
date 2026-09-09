App({
  onLaunch() {
    // 初始化云开发
    try {
      wx.cloud.init({
        env: "cloud1-3gf9as2scb7946d3", // 请替换为你的云开发环境ID
        traceUser: true
      });
      console.log('云开发初始化完成');
      
      // 检查云开发是否初始化成功
      const db = wx.cloud.database();
      console.log('云开发数据库实例创建成功');
    } catch (error) {
      console.error('云开发初始化失败:', error);
      wx.showModal({
        title: '初始化失败',
        content: '云开发初始化失败，请检查环境ID是否正确',
        showCancel: false
      });
    }
  }
});
