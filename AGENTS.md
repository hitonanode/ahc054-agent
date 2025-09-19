# Instruction

AtCoder で開催されている AtCoder Heuristic Contest を解くためのプログラムを作るリポジトリです。

## ディレクトリ構成

- 今回のコンテストの問題文は ./problem_statement.html です。
- ./main.cpp に解法を実装します。

## コーディング規約

- 解答は C++ で作成する。 main.cpp に全てを実装する
- `#include <bits/stdc++.h>` などは用いない
- C++20 準拠で実装する
- include は適切に行い、 GCC でも Clang でもビルドできるようにする
- 乱数の初期 seed は定数として埋め込んで、複数回の実行で再現性を担保する
- 最大数十行程度を目安に、 stderr に実行経過を出力してもよい

## 注意点

以下を厳守する。

- Chat は日本語で回答してください

## アルゴリズムの方針

STRATEGY.md に詳細に記載されています。この方針に従ってください
