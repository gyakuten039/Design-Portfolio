const cloud = require('wx-server-sdk')
cloud.init({
  env: cloud.DYNAMIC_CURRENT_ENV
})

// ===== 配置区域（用户需要填的部分）=====
const DOUBAO_API_KEY = "*****"; // 你的 API Key
const DOUBAO_MODEL = "doubao-seedream-4-0-250828"; // 模型ID（Doubao-Seedream-4.0）
const DOUBAO_API_URL = "https://ark.cn-beijing.volces.com/api/v3/images/generations"; // API地址
// =====================================

exports.main = async (event, context) => {
  const { prompt, imageUrl, mode } = event
  
  if (!prompt && !imageUrl) {
    return {
      success: false,
      error: '缺少参数，请提供 prompt 或 imageUrl'
    }
  }

  console.log('=== 开始处理图片请求 ===')
  console.log('模式:', mode || '文生图')
  console.log('Prompt:', prompt)
  console.log('图片URL:', imageUrl || '无')
  console.log('使用模型: 豆包 Doubao-Seedream-4.0 (比赛第6个AI)')

  try {
    // 根据模式调用不同的API
    let result;
    
    if (mode === 'img2img' && imageUrl) {
      // ===== 图生图模式 =====
      console.log('【图生图模式】调用豆包API...')
      result = await callDoubaoImageAPI(prompt, imageUrl);
    } else {
      // ===== 文生图模式 =====
      console.log('【文生图模式】调用豆包API...')
      result = await callDoubaoImageAPI(prompt);
    }
    
    if (result.success) {
      console.log('✅ 豆包生图成功！')
      return {
        success: true,
        imageUrl: result.imageUrl,
        method: 'doubao-seedream-4.0',
        model: '豆包 Doubao-Seedream-4.0 (比赛第6个AI)',
        mode: mode || '文生图'
      }
    } else {
      console.log('⚠️ 豆包失败:', result.error)
      return {
        success: false,
        error: result.error
      }
    }
    
  } catch (error) {
    console.error('❌ 生图失败:', error)
    return {
      success: false,
      error: error.message
    }
  }
}

// 调用豆包图像生成API（支持文生图和图生图）
async function callDoubaoImageAPI(prompt, imageUrl = null) {
  try {
    const https = require('https')
    const url = new URL(DOUBAO_API_URL)
    
    // 构建请求数据
    let requestData = {
      model: DOUBAO_MODEL,
      n: 1,
      size: "2K", // 2048x2048 高清
      response_format: "url",
      stream: false
    }
    
    // 如果有图片URL，使用图生图模式
    if (imageUrl) {
      // 豆包支持在prompt中传入图片URL实现图生图
      requestData.prompt = `请根据这张图片生成一张中国传统剪纸艺术风格的图片：${imageUrl}\n\n风格要求：${prompt || '保持原图的主要元素，转换为红色剪纸风格，白色背景，精细镂空，对称构图'}`
    } else {
      requestData.prompt = prompt
    }
    
    const postData = JSON.stringify(requestData)
    
    const options = {
      hostname: url.hostname,
      port: 443,
      path: url.pathname,
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${DOUBAO_API_KEY}`,
        'Content-Length': Buffer.byteLength(postData)
      }
    }

    return new Promise((resolve, reject) => {
      const req = https.request(options, (res) => {
        let data = ''
        
        res.on('data', (chunk) => {
          data += chunk
        })
        
        res.on('end', () => {
          try {
            const result = JSON.parse(data)
            console.log('豆包API返回:', JSON.stringify(result).substring(0, 500))
            
            if (result.data && result.data[0] && result.data[0].url) {
              resolve({
                success: true,
                imageUrl: result.data[0].url
              })
            } else if (result.error) {
              resolve({
                success: false,
                error: result.error.message || '豆包API返回错误'
              })
            } else {
              resolve({
                success: false,
                error: '未获取到图片URL'
              })
            }
          } catch (e) {
            reject(new Error('解析响应失败: ' + e.message))
          }
        })
      })

      req.on('error', (e) => {
        reject(new Error('请求失败: ' + e.message))
      })

      req.write(postData)
      req.end()
    })
    
  } catch (error) {
    return {
      success: false,
      error: error.message
    }
  }
}
