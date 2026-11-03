# Bilibili_UID_Checker

一个自动检测随机 Bilibili UID 是否为曾经注册机的垃圾账号的工具。

## 功能

- 随机生成 Bilibili UID 并检测账号状态
- 可限定 UID 的开头数字
- 限定 UID 的长度
- 当账号满足以下条件时判定为“空账号”：
  - 等级为 0
  - 用户名由纯小写字母组成，或由小写字母 + 数字组成
  - 用户名不包含 `black_list_substr.txt` 中的关键词
- 命中的 UID 保存到 `result.txt`


运行后按提示输入：

- UID 开头（可选，不填则随机）
- UID 长度（建议 7 位或更长）

## 数据来源

调用接口：

```
https://bili.shoyu.top/api/bili4.php?type=card&uid=1234567
```

该接口可能触发风控，如遇到请求失败，请更换 IP 后重试。

## 输出

检测到的空账号 UID 会写入 `result.txt`。

## 后续验证

对 `result.txt` 中的 UID，可以尝试用 `用户名@qq.com` 或 `用户名@163.com` 作为重置密码邮箱：

- 如果能够查到该账户 → 可以尝试注册该邮箱并找回密码
- 如果提示 `UP user not found` → 换下一个 UID 尝试

## 配置

- `black_list_substr.txt`：每行一个关键词，用户名包含任一关键词即被排除

## 免责声明

本项目仅供学习与技术研究使用。请遵守 Bilibili 用户协议及相关法律法规，勿用于任何侵犯他人权益的用途。因使用本项目产生的一切后果由使用者自行承担。


## Star History

<a href="https://www.star-history.com/?repos=sherlock-qxy%2Fbilibili_uid_checker&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=sherlock-qxy/bilibili_uid_checker&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=sherlock-qxy/bilibili_uid_checker&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=sherlock-qxy/bilibili_uid_checker&type=date&legend=top-left" />
 </picture>
</a>
