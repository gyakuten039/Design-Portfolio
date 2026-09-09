// 剪纸AI问答页面
Page({
  data: {
    inputValue: '',
    chatList: [],
    isGeneratingText: false,
    isGeneratingImage: false,
    currentModel: 'ernie-3.5-8k-preview', // 生文模型
    imageFunctionName: 'generatePaperCutImage', // 生图云函数名称
    cloudInitialized: false, // 云开发是否初始化成功
    canSend: false, // 是否可以发送
    canGenerateImage: false, // 是否可以生成图片
    scrollToView: '', // 滚动到的位置
    selectedImage: '' // 已选择的图片路径
  },

  // 用于节流更新
  updateTimer: null,
  pendingContent: '',
  lastUpdateTime: 0,

  onLoad() {
    // 页面加载时检查云开发状态
    this.checkCloudStatus();
  },

  // 检查云开发状态
  checkCloudStatus() {
    try {
      // 尝试获取云开发实例
      const db = wx.cloud.database();
      this.setData({
        cloudInitialized: true
      });
      console.log('云开发状态检查：已初始化');
    } catch (error) {
      console.error('云开发状态检查：未初始化', error);
      wx.showModal({
        title: '提示',
        content: '云开发未正确初始化，请检查：\n1. 是否已开通云开发\n2. app.js中的环境ID是否正确',
        showCancel: false
      });
    }
  },

  // 使用示例问题
  useExample(e) {
    const text = e.currentTarget.dataset.text;
    this.setData({
      inputValue: text,
      canSend: true
    });
  },

  // 输入框内容变化
  onInputChange(e) {
    const value = e.detail.value;
    const canSend = (value.trim().length > 0 || this.data.selectedImage) && !this.data.isGeneratingText;
    this.setData({
      inputValue: value,
      canSend: canSend
    });
  },

  // 更新按钮状态
  updateButtonStatus() {
    const { inputValue, isGeneratingText, chatList, isGeneratingImage, cloudInitialized, selectedImage } = this.data;
    
    // 检查是否可以发送：有内容或已选图片、不在生成中、云开发已初始化
    const canSend = (inputValue.trim().length > 0 || selectedImage) && !isGeneratingText && cloudInitialized;
    
    // 检查是否可以生成图片：有AI回复、不在生成中、云开发已初始化
    const hasAiMessage = chatList.some(msg => msg.type === 'ai' && msg.content);
    const canGenerateImage = hasAiMessage && !isGeneratingImage && cloudInitialized;
    
    this.setData({
      canSend,
      canGenerateImage
    });
  },

  // 发送消息 - 调用生文模型
  async sendMessage() {
    const { inputValue, chatList, canSend, selectedImage } = this.data;
    
    // 检查是否可以发送
    if (!canSend) {
      if (!inputValue.trim() && !selectedImage) {
        wx.showToast({
          title: '请输入内容或选择图片',
          icon: 'none'
        });
      }
      return;
    }

    // 构建用户消息内容
    let userContent = inputValue.trim();
    let messageType = 'user';
    let imagePath = '';

    // 如果有图片，将图片信息加入消息
    if (selectedImage) {
      imagePath = selectedImage;
      if (!userContent) {
        userContent = '📷 [图片]';
      }
      messageType = 'user-image';
    }

    // 添加用户消息
    const userMessage = {
      type: messageType,
      content: userContent,
      imagePath: imagePath,
      time: new Date().toLocaleTimeString()
    };

    // 保存要发送的内容，因为setData会清空inputValue
    const textToSend = userContent;

    this.setData({
      chatList: [...chatList, userMessage],
      inputValue: '',
      selectedImage: '',
      isGeneratingText: true,
      canSend: false
    });

    try {
      // 调用生文模型（如果有图片，传入图片路径）
      await this.callTextModel(textToSend, imagePath);
    } catch (error) {
      console.error('生文调用失败:', error);
      wx.showModal({
        title: '生成失败',
        content: '错误信息：' + (error.message || JSON.stringify(error)),
        showCancel: false
      });
    } finally {
      this.setData({
        isGeneratingText: false
      });
      this.updateButtonStatus();
    }
  },

  // 调用生文模型
  async callTextModel(userInput, imagePath = '') {
    const { chatList, currentModel } = this.data;
    
    // 创建AI消息占位
    const aiMessage = {
      type: 'ai',
      content: '',
      time: new Date().toLocaleTimeString(),
      isStreaming: true
    };
    
    this.setData({
      chatList: [...chatList, aiMessage]
    });

    try {
      console.log('callTextModel 被调用，userInput:', userInput, 'imagePath:', imagePath);
      
      // 如果有图片，直接在前端调用支持图片的模型
      if (imagePath) {
        console.log('检测到图片，使用豆包视觉模型进行识别...');

        try {
          // 1. 上传图片到云存储
          console.log('开始上传图片到云存储...');
          const uploadResult = await wx.cloud.uploadFile({
            cloudPath: `images/${Date.now()}.jpg`,
            filePath: imagePath
          });

          console.log('图片上传结果:', uploadResult);
          const cloudFileID = uploadResult.fileID;

          // 2. 获取临时访问 URL
          const tempUrlResult = await wx.cloud.getTempFileURL({
            fileList: [cloudFileID]
          });

          const tempFileURL = tempUrlResult.fileList[0].tempFileURL;
          console.log('图片临时 URL:', tempFileURL);

          // 3. 调用 recognizeImage 云函数（使用豆包视觉模型）
          console.log('调用 recognizeImage 云函数...');
          const recognizeRes = await wx.cloud.callFunction({
            name: 'recognizeImage',
            data: {
              imageUrl: tempFileURL,
              question: userInput || "请详细描述这张图片的内容、风格和特点"
            }
          });

          console.log('识别结果:', recognizeRes);

          if (recognizeRes.result && recognizeRes.result.success) {
            // 成功获取识别结果
            const content = recognizeRes.result.content;
            console.log('✅ 图片识别成功！');

            // 直接显示识别结果（非流式）
            const finalChatList = [...this.data.chatList];
            const lastIndex = finalChatList.length - 1;
            if (lastIndex >= 0 && finalChatList[lastIndex].type === 'ai') {
              finalChatList[lastIndex].content = content;
              finalChatList[lastIndex].isStreaming = false;
              this.setData({
                chatList: finalChatList,
                scrollToView: 'msg-' + (finalChatList.length - 1)
              });
            }
            return;
          } else {
            console.error('识别失败:', recognizeRes.result?.error);
            throw new Error(recognizeRes.result?.error || '识别失败');
          }

        } catch (imageError) {
          console.error('图片识别失败:', imageError);
          // 显示错误信息
          const errorChatList = [...this.data.chatList];
          const errorLastIndex = errorChatList.length - 1;
          if (errorLastIndex >= 0 && errorChatList[errorLastIndex].type === 'ai') {
            errorChatList[errorLastIndex].content = '抱歉，图片识别失败：' + (imageError.message || '未知错误') + '\n\n可能原因：\n1. 豆包API未配置或Key无效\n2. 图片格式不支持\n3. 网络连接问题';
            errorChatList[errorLastIndex].isStreaming = false;
            this.setData({
              chatList: errorChatList
            });
          }
          return;
        }
      }
      
      // 构建消息内容
      let messages = [
        {
          role: "system",
          content: "你是一位专业的剪纸艺术专家，擅长回答关于中国传统剪纸艺术的各种问题，包括剪纸的历史、技法、图案寓意、制作步骤等。请用专业且易懂的语言回答。"
        }
      ];

      messages.push({
        role: "user",
        content: userInput
      });

      console.log('调用AI模型，消息:', JSON.stringify(messages));

      // 调用云开发AI能力
      let res;
      try {
        res = await wx.cloud.extend.AI.createModel(
          "qianfan-custom"
        ).streamText({
          data: {
            model: currentModel,
            messages: messages
          }
        });
        
        console.log('AI响应对象:', res);
      } catch (modelError) {
        console.error('调用AI模型失败:', modelError);
        throw modelError;
      }

      let fullContent = '';
      let lastScrollTime = 0;
      const scrollInterval = 1000; // 每1000ms滚动一次
      const updateInterval = 300; // 每300ms更新一次界面
      
      // 处理流式响应
      console.log('开始处理流式响应...');
      for await (let event of res.eventStream) {
        console.log('收到事件:', event);
        if (event.data === "[DONE]") {
          console.log('收到 [DONE]，结束流式响应');
          break;
        }
        
        try {
          const data = JSON.parse(event.data);
          console.log('解析后的数据:', data);
          
          // 处理思维链内容（如使用 deepseek-r1）
          const think = data?.choices?.[0]?.delta?.reasoning_content;
          if (think) {
            console.log('思维链:', think);
          }
          
          // 获取实际内容
          const content = data?.choices?.[0]?.delta?.content;
          console.log('提取到的内容:', content);
          if (content) {
            fullContent += content;
            
            // 使用节流更新UI，每300ms更新一次（让生成过程更慢更流畅）
            const now = Date.now();
            if (now - this.lastUpdateTime > updateInterval) {
              this.updateAIContent(fullContent);
              this.lastUpdateTime = now;
            } else {
              // 缓存内容，延迟更新
              this.pendingContent = fullContent;
              if (!this.updateTimer) {
                this.updateTimer = setTimeout(() => {
                  this.updateAIContent(this.pendingContent);
                  this.updateTimer = null;
                }, updateInterval);
              }
            }

            // 控制滚动频率
            const scrollNow = Date.now();
            if (scrollNow - lastScrollTime > scrollInterval) {
              this.scrollToBottom();
              lastScrollTime = scrollNow;
            }
          }
        } catch (e) {
          console.error('解析响应失败:', e);
        }
      }

      // 确保最后的内容被更新
      console.log('流式响应结束，最终内容:', fullContent);
      if (this.updateTimer) {
        clearTimeout(this.updateTimer);
        this.updateTimer = null;
      }
      this.updateAIContent(fullContent);

      // 标记流式结束
      const finalChatList = [...this.data.chatList];
      const lastIndex = finalChatList.length - 1;
      if (lastIndex >= 0 && finalChatList[lastIndex].type === 'ai') {
        finalChatList[lastIndex].isStreaming = false;
        
        this.setData({
          chatList: finalChatList,
          scrollToView: 'msg-' + (finalChatList.length - 1)
        });
      }
    } catch (error) {
      console.error('调用生文模型失败:', error);
      // 更新AI消息显示错误
      const newChatList = [...this.data.chatList];
      const lastIndex = newChatList.length - 1;
      if (lastIndex >= 0 && newChatList[lastIndex].type === 'ai') {
        newChatList[lastIndex].content = '抱歉，生成回答时出现错误：' + (error.message || '未知错误');
        newChatList[lastIndex].isStreaming = false;
        
        this.setData({
          chatList: newChatList
        });
      }
    }
  },

  // 更新AI消息内容（节流）
  updateAIContent(content) {
    const newChatList = [...this.data.chatList];
    const lastIndex = newChatList.length - 1;
    console.log('updateAIContent - lastIndex:', lastIndex, 'type:', lastIndex >= 0 ? newChatList[lastIndex].type : 'none');
    if (lastIndex >= 0 && newChatList[lastIndex].type === 'ai') {
      newChatList[lastIndex].content = content;
      this.setData({
        chatList: newChatList
      });
    } else {
      console.warn('无法更新AI内容，最后一条消息不是AI类型');
    }
  },

  // 滚动到底部
  scrollToBottom() {
    const { chatList } = this.data;
    if (chatList.length > 0) {
      this.setData({
        scrollToView: 'msg-' + (chatList.length - 1)
      });
    }
  },

  // 生成剪纸图片 - 调用云函数（主方案：豆包 + 备用：Pollinations）
  async generatePaperCutImage() {
    const { chatList, canGenerateImage } = this.data;

    // 检查是否可以生成
    if (!canGenerateImage) {
      wx.showToast({ title: '请先生成文字内容', icon: 'none' });
      return;
    }

    // 获取最后一条AI回复作为图片生成提示
    const lastAiMessage = chatList.slice().reverse().find(msg => msg.type === 'ai');
    if (!lastAiMessage || !lastAiMessage.content) {
      wx.showToast({ title: '请先生成文字内容', icon: 'none' });
      return;
    }

    this.setData({ isGeneratingImage: true, canGenerateImage: false });
    wx.showLoading({ title: '正在生成图片', mask: true });

    try {
      // 构建专业的剪纸提示词
      const prompt = `纯中国传统剪纸艺术，红色剪纸，白色背景，${lastAiMessage.content.substring(0, 150)}，精细镂空雕刻，对称构图，民间艺术风格，精致细节，不要任何文字说明，不要水印，纯图案剪纸，传统窗花风格`;

      console.log('调用云函数生图, prompt:', prompt);

      // 调用云函数（内部会尝试豆包，失败则自动切换到Pollinations）
      const res = await wx.cloud.callFunction({
        name: 'generateImage',
        data: { prompt: prompt, mode: 'txt2img' }
      });

      console.log('云函数返回结果:', res);

      if (res.result?.success && res.result?.imageUrl) {
        // 成功获取到图片URL
        const imageUrl = res.result.imageUrl;

        console.log('✅ 图片生成成功！使用模型:', res.result.model);

        // 添加图片消息到聊天列表
        const imageMessage = {
          type: 'image',
          imageUrl: imageUrl,
          prompt: prompt,
          time: new Date().toLocaleTimeString(),
          method: res.result.method || 'cloud-ai',
          model: res.result.model || '未知'
        };

        this.setData({ chatList: [...this.data.chatList, imageMessage] });

        // 显示使用的模型信息
        wx.showToast({
          title: `🎨 ${res.result.method.includes('doubao') ? '豆包' : 'Flux'} 生图成功！`,
          icon: 'success',
          duration: 2000
        });

      } else {
        throw new Error(res.result?.error || '云函数返回错误');
      }

    } catch (error) {
      console.error('❌ 生图失败:', error);

      wx.showModal({
        title: '⚠️ 图片生成失败',
        content: `${error.message || '未知错误'}\n\n📌 已配置：豆包(第6个AI) + Pollinations(备用)`,
        confirmText: '重试',
        cancelText: '取消',
        success: (modalRes) => {
          if (modalRes.confirm) this.generatePaperCutImage();
        }
      });
    } finally {
      wx.hideLoading();
      this.setData({ isGeneratingImage: false });
      this.updateButtonStatus();
    }
  },

  // 图生图功能 - 基于选中图片生成新图片（使用豆包 API）
  async generateImageFromImage() {
    const { selectedImage, inputValue, isGeneratingImage } = this.data;

    // 检查是否有选中图片
    if (!selectedImage) {
      wx.showToast({ title: '请先选择一张图片', icon: 'none' });
      return;
    }

    if (isGeneratingImage) {
      wx.showToast({ title: '正在生成中，请稍候', icon: 'none' });
      return;
    }

    // 如果有输入文字，检查是否有AI回复可以使用
    let prompt = inputValue.trim();
    if (!prompt) {
      // 如果没有输入，使用默认提示词
      prompt = '将这张图片转换为中国传统剪纸艺术风格，保持主要元素，红色剪纸，白色背景，精细镂空';
    }

    wx.showModal({
      title: '确认生成',
      content: '是否基于选中的图片生成剪纸风格的新图片？',
      success: async (res) => {
        if (res.confirm) {
          await this.doGenerateImageFromImage(selectedImage, prompt);
        }
      }
    });
  },

  // 执行图生图
  async doGenerateImageFromImage(imagePath, prompt) {
    this.setData({ isGeneratingImage: true });
    wx.showLoading({ title: '正在生成图片', mask: true });

    try {
      console.log('=== 开始图生图 ===');
      console.log('原始图片:', imagePath);
      console.log('提示词:', prompt);

      // 1. 先上传原始图片到云存储
      console.log('步骤1: 上传原始图片到云存储...');
      const uploadResult = await wx.cloud.uploadFile({
        cloudPath: `original-images/${Date.now()}.jpg`,
        filePath: imagePath
      });

      const cloudFileID = uploadResult.fileID;
      console.log('图片上传成功, fileID:', cloudFileID);

      // 2. 获取临时访问URL
      console.log('步骤2: 获取临时访问URL...');
      const tempUrlResult = await wx.cloud.getTempFileURL({
        fileList: [cloudFileID]
      });

      const imageUrl = tempUrlResult.fileList[0].tempFileURL;
      console.log('图片临时URL:', imageUrl);

      if (!imageUrl) {
        throw new Error('无法获取图片访问URL');
      }

      // 3. 调用云函数进行图生图
      console.log('步骤3: 调用云函数进行图生图...');
      const res = await wx.cloud.callFunction({
        name: 'generateImage',
        data: {
          prompt: prompt,
          imageUrl: imageUrl,
          mode: 'img2img'
        }
      });

      console.log('云函数返回:', res);

      if (res.result?.success && res.result?.imageUrl) {
        // 成功生成新图片
        const newImageUrl = res.result.imageUrl;

        console.log('✅ 图生图成功！');

        // 添加原始图片和新图片到聊天列表
        const resultMessage = {
          type: 'image-generation',
          originalImage: imagePath,
          originalCloudUrl: imageUrl,
          generatedImage: newImageUrl,
          prompt: prompt,
          time: new Date().toLocaleTimeString(),
          method: res.result.method || 'cloud-ai',
          model: res.result.model || '未知'
        };

        this.setData({
          chatList: [...this.data.chatList, resultMessage],
          selectedImage: '', // 清空已选图片
          inputValue: '' // 清空输入
        });

        wx.showToast({
          title: `🎨 ${res.result.method.includes('doubao') ? '豆包' : 'Flux'} 图生图成功！`,
          icon: 'success',
          duration: 2000
        });

      } else {
        throw new Error(res.result?.error || '图生图失败');
      }

    } catch (error) {
      console.error('❌ 图生图失败:', error);

      wx.showModal({
        title: '⚠️ 图生图失败',
        content: `${error.message || '未知错误'}\n\n📌 使用模型：豆包 Doubao-Seedream-4.0 (比赛第6个AI)`,
        confirmText: '重试',
        cancelText: '取消',
        success: (modalRes) => {
          if (modalRes.confirm) {
            this.doGenerateImageFromImage(imagePath, prompt);
          }
        }
      });
    } finally {
      wx.hideLoading();
      this.setData({ isGeneratingImage: false });
      this.updateButtonStatus();
    }
  },

  // 保存图片到相册
  async saveImage(e) {
    const { url } = e.currentTarget.dataset;
    
    try {
      // 下载图片
      const downloadRes = await wx.downloadFile({
        url: url
      });

      // 保存到相册
      await wx.saveImageToPhotosAlbum({
        filePath: downloadRes.tempFilePath
      });

      wx.showToast({
        title: '保存成功',
        icon: 'success'
      });
    } catch (error) {
      console.error('保存失败:', error);
      wx.showToast({
        title: '保存失败',
        icon: 'none'
      });
    }
  },

  // 预览图片
  previewImage(e) {
    const { url } = e.currentTarget.dataset;
    wx.previewImage({
      urls: [url],
      current: url
    });
  },

  // 清空对话
  clearChat() {
    wx.showModal({
      title: '提示',
      content: '确定要清空所有对话吗？',
      success: (res) => {
        if (res.confirm) {
          this.setData({
            chatList: []
          });
          this.updateButtonStatus();
        }
      }
    });
  },

  // 选择照片
  async selectImage() {
    try {
      // 选择图片
      const res = await wx.chooseImage({
        count: 1,
        sizeType: ['original', 'compressed'],
        sourceType: ['album', 'camera'],
      });

      const tempFilePaths = res.tempFilePaths;
      if (tempFilePaths && tempFilePaths.length > 0) {
        const imagePath = tempFilePaths[0];
        
        // 将图片保存到输入区域状态，不自动发送
        this.setData({
          selectedImage: imagePath
        });
        
        // 更新按钮状态
        this.updateButtonStatus();
        
        wx.showToast({
          title: '图片已选择，请输入文字后发送',
          icon: 'none',
          duration: 2000
        });
      }
    } catch (error) {
      console.error('选择照片失败:', error);
      wx.showToast({
        title: '选择照片失败',
        icon: 'none'
      });
    }
  },

  // 取消选择照片
  cancelSelectImage() {
    this.setData({
      selectedImage: ''
    });
    this.updateButtonStatus()
  }
});
