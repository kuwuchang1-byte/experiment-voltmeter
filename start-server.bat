@echo off
cd /d E:\HanakoWorks\experiment-voltmeter
node -e "const h=require('http'),f=require('fs');h.createServer((q,r)=>{r.writeHead(200,{'Content-Type':'text/html;charset=utf-8'});r.end(f.readFileSync('index.html','utf8'))}).listen(8080,()=>console.log('服务器已启动: http://localhost:8080'))"
pause
