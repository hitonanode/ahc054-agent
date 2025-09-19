#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <array>
#include <utility>
#include <algorithm>

struct Solver {
    int N = 0;
    int ti = 0;
    int tj = 0;
    int si = 0;
    int sj = 0;
    std::vector<std::string> initial;
    std::vector<std::string> board;
    std::vector<std::vector<bool>> confirmed;
    bool preprocessed = false;
    const std::array<int, 4> dx{-1, 1, 0, 0};
    const std::array<int, 4> dy{0, 0, -1, 1};

    void init() {
        std::cin >> N >> ti >> tj;
        initial.resize(N);
        for (int i = 0; i < N; ++i) {
            std::cin >> initial[i];
        }
        board = initial;
        confirmed.assign(N, std::vector<bool>(N, false));
        si = 0;
        sj = N / 2;
        confirmed[si][sj] = true;
    }

    bool inside(int x, int y) const {
        return 0 <= x && x < N && 0 <= y && y < N;
    }

    int manhattanDistance(int x1, int y1, int x2, int y2) const {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    bool maintainsConnectivity() const {
        if (board[si][sj] != '.') {
            return false;
        }
        std::queue<std::pair<int, int>> q;
        std::vector<std::vector<bool>> visited(N, std::vector<bool>(N, false));
        visited[si][sj] = true;
        q.emplace(si, sj);
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (visited[nx][ny]) {
                    continue;
                }
                if (board[nx][ny] != '.') {
                    continue;
                }
                visited[nx][ny] = true;
                q.emplace(nx, ny);
            }
        }
        if (!visited[ti][tj]) {
            return false;
        }
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (board[i][j] == '.' && !visited[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool canPlace(int x, int y, int pi, int pj) const {
        if (!inside(x, y)) {
            return false;
        }
        if (confirmed[x][y]) {
            return false;
        }
        if (board[x][y] != '.') {
            return false;
        }
        if (x == pi && y == pj) {
            return false;
        }
        if (x == ti && y == tj) {
            return false;
        }
        return true;
    }

    bool tryPlace(int x, int y, int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        if (!canPlace(x, y, pi, pj)) {
            return false;
        }
        board[x][y] = 'T';
        if (!maintainsConnectivity()) {
            board[x][y] = '.';
            return false;
        }
        placements.emplace_back(x, y);
        return true;
    }

    template <class Predicate>
    std::vector<std::pair<int, int>> shortestPath(int sx, int sy, int gx, int gy, Predicate allow) const {
        if (!inside(sx, sy) || !inside(gx, gy)) {
            return {};
        }
        if (!allow(sx, sy) || !allow(gx, gy)) {
            return {};
        }
        std::queue<std::pair<int, int>> q;
        std::vector<std::vector<std::pair<int, int>>> parent(N, std::vector<std::pair<int, int>>(N, {-1, -1}));
        parent[sx][sy] = {sx, sy};
        q.emplace(sx, sy);
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            if (x == gx && y == gy) {
                break;
            }
            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (!allow(nx, ny)) {
                    continue;
                }
                if (parent[nx][ny].first != -1) {
                    continue;
                }
                parent[nx][ny] = {x, y};
                q.emplace(nx, ny);
            }
        }
        if (parent[gx][gy].first == -1) {
            return {};
        }
        std::vector<std::pair<int, int>> path;
        int cx = gx;
        int cy = gy;
        while (!(cx == sx && cy == sy)) {
            path.emplace_back(cx, cy);
            auto [px, py] = parent[cx][cy];
            cx = px;
            cy = py;
        }
        path.emplace_back(sx, sy);
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<std::vector<int>> bfsDistance(int sx, int sy) const {
        std::vector<std::vector<int>> dist(N, std::vector<int>(N, -1));
        if (!inside(sx, sy) || board[sx][sy] != '.') {
            return dist;
        }
        std::queue<std::pair<int, int>> q;
        q.emplace(sx, sy);
        dist[sx][sy] = 0;
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();
            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (board[nx][ny] != '.') {
                    continue;
                }
                if (dist[nx][ny] != -1) {
                    continue;
                }
                dist[nx][ny] = dist[x][y] + 1;
                q.emplace(nx, ny);
            }
        }
        return dist;
    }

    bool frontConeClear(int dir, int dist, int pi, int pj) const {
        if (dist <= 0) {
            return false;
        }
        int baseX = pi + dx[dir] * (dist - 1);
        int baseY = pj + dy[dir] * (dist - 1);
        if (dir < 2) { // vertical
            for (int deltaY = -1; deltaY <= 1; ++deltaY) {
                int nx = baseX;
                int ny = baseY + deltaY;
                if (!inside(nx, ny)) {
                    return false;
                }
                if (board[nx][ny] == 'T') {
                    return false;
                }
            }
        } else { // horizontal
            for (int deltaX = -1; deltaX <= 1; ++deltaX) {
                int nx = baseX + deltaX;
                int ny = baseY;
                if (!inside(nx, ny)) {
                    return false;
                }
                if (board[nx][ny] == 'T') {
                    return false;
                }
            }
        }
        return true;
    }

    void placeWithFrontPreference(int dir, int pi, int pj, std::vector<std::pair<int, int>> &placements,
                                  const std::vector<int> &preferredDistances, int fallbackStart) {
        bool placed = false;
        for (int dist : preferredDistances) {
            int nx = pi + dx[dir] * dist;
            int ny = pj + dy[dir] * dist;
            if (!inside(nx, ny)) {
                break;
            }
            if (board[nx][ny] == 'T') {
                break;
            }
            if (!canPlace(nx, ny, pi, pj)) {
                continue;
            }
            if (!frontConeClear(dir, dist, pi, pj)) {
                continue;
            }
            if (tryPlace(nx, ny, pi, pj, placements)) {
                placed = true;
                break;
            }
        }
        if (placed) {
            return;
        }
        for (int dist = fallbackStart;; ++dist) {
            int nx = pi + dx[dir] * dist;
            int ny = pj + dy[dir] * dist;
            if (!inside(nx, ny)) {
                break;
            }
            if (board[nx][ny] == 'T') {
                break;
            }
            if (!canPlace(nx, ny, pi, pj)) {
                continue;
            }
            if (tryPlace(nx, ny, pi, pj, placements)) {
                break;
            }
        }
    }

    void handleConfirmedFront(int dir, int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        int adjX = pi + dx[dir];
        int adjY = pj + dy[dir];
        if (!inside(adjX, adjY)) {
            return;
        }
        if (!confirmed[adjX][adjY]) {
            return;
        }
        if (board[adjX][adjY] == 'T') {
            return;
        }
        for (int dist = 2;; ++dist) {
            int nx = pi + dx[dir] * dist;
            int ny = pj + dy[dir] * dist;
            if (!inside(nx, ny)) {
                break;
            }
            if (board[nx][ny] == 'T') {
                break;
            }
            if (confirmed[nx][ny]) {
                continue;
            }
            if (tryPlace(nx, ny, pi, pj, placements)) {
                break;
            }
        }
    }

    void blockLineToTarget(int pi, int pj, int tx, int ty, std::vector<std::pair<int, int>> &placements) {
        if (pi == tx) {
            int step = (ty > pj) ? 1 : -1;
            bool blocked = false;
            for (int y = pj + step; y != ty; y += step) {
                if (board[pi][y] == 'T') {
                    blocked = true;
                    break;
                }
            }
            if (blocked) {
                return;
            }
            for (int y = pj + step; y != ty; y += step) {
                if (tryPlace(pi, y, pi, pj, placements)) {
                    return;
                }
            }
        } else if (pj == ty) {
            int step = (tx > pi) ? 1 : -1;
            bool blocked = false;
            for (int x = pi + step; x != tx; x += step) {
                if (board[x][pj] == 'T') {
                    blocked = true;
                    break;
                }
            }
            if (blocked) {
                return;
            }
            for (int x = pi + step; x != tx; x += step) {
                if (tryPlace(x, pj, pi, pj, placements)) {
                    return;
                }
            }
        }
    }

    void blockLineToFlower(int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        blockLineToTarget(pi, pj, ti, tj, placements);
    }

    void blockLineToFlowerNeighbors(int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        for (int dir = 0; dir < 4; ++dir) {
            int ax = ti + dx[dir];
            int ay = tj + dy[dir];
            if (!inside(ax, ay)) {
                continue;
            }
            if (board[ax][ay] == 'T') {
                continue;
            }
            int moveDir = -1;
            if (pi == ax) {
                moveDir = (ay > pj) ? 3 : 2; // right or left
            } else if (pj == ay) {
                moveDir = (ax > pi) ? 1 : 0; // down or up
            }
            if (moveDir == -1) {
                continue;
            }
            bool blocked = false;
            int cx = pi + dx[moveDir];
            int cy = pj + dy[moveDir];
            while (inside(cx, cy)) {
                if (cx == ax && cy == ay) {
                    break;
                }
                if (board[cx][cy] == 'T') {
                    blocked = true;
                    break;
                }
                cx += dx[moveDir];
                cy += dy[moveDir];
            }
            if (blocked) {
                continue;
            }
            if (cx != ax || cy != ay) {
                continue;
            }
            cx = pi + dx[moveDir];
            cy = pj + dy[moveDir];
            while (inside(cx, cy)) {
                if (tryPlace(cx, cy, pi, pj, placements)) {
                    break;
                }
                if (cx == ax && cy == ay) {
                    break;
                }
                cx += dx[moveDir];
                cy += dy[moveDir];
            }
        }
    }

    void handleAdjacentToFlower(int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        for (int dir = 0; dir < 4; ++dir) {
            int nx = pi + dx[dir];
            int ny = pj + dy[dir];
            if (!inside(nx, ny)) {
                continue;
            }
            if (board[nx][ny] != '.') {
                continue;
            }
            if (confirmed[nx][ny]) {
                continue;
            }
            if (std::abs(nx - ti) + std::abs(ny - tj) == 1) {
                tryPlace(nx, ny, pi, pj, placements);
            }
        }
    }

    void handleAdjacentSquares(int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        const std::array<std::pair<int, int>, 4> squares{
            std::make_pair(pi - 1, pj - 1),
            std::make_pair(pi - 1, pj),
            std::make_pair(pi, pj - 1),
            std::make_pair(pi, pj)
        };
        for (auto [sx, sy] : squares) {
            int ex = sx + 1;
            int ey = sy + 1;
            if (!(inside(sx, sy) && inside(ex, sy) && inside(sx, ey) && inside(ex, ey))) {
                continue;
            }
            std::array<std::pair<int, int>, 4> cells{
                std::make_pair(sx, sy),
                std::make_pair(ex, sy),
                std::make_pair(sx, ey),
                std::make_pair(ex, ey)
            };
            bool allOpen = true;
            bool allConfirmedNow = true;
            for (auto [cx, cy] : cells) {
                if (board[cx][cy] != '.') {
                    allOpen = false;
                    break;
                }
                if (!confirmed[cx][cy]) {
                    allConfirmedNow = false;
                }
            }
            if (!allOpen || allConfirmedNow) {
                continue;
            }
            bool placed = false;
            for (int dir = 0; dir < 4; ++dir) {
                int nx = pi + dx[dir];
                int ny = pj + dy[dir];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (nx < sx || nx > ex || ny < sy || ny > ey) {
                    continue;
                }
                if (confirmed[nx][ny]) {
                    continue;
                }
                if (board[nx][ny] != '.') {
                    continue;
                }
                if (tryPlace(nx, ny, pi, pj, placements)) {
                    placed = true;
                    break;
                }
            }
            (void)placed;
        }
    }

    void preprocessInitial(int pi, int pj, std::vector<std::pair<int, int>> &placements) {
        auto allowAll = [this](int x, int y) {
            return board[x][y] == '.';
        };
        auto allowFar2 = [this](int x, int y) {
            if (board[x][y] != '.') {
                return false;
            }
            return manhattanDistance(x, y, ti, tj) >= 2;
        };
        auto allowFar5 = [this](int x, int y) {
            if (board[x][y] != '.') {
                return false;
            }
            return manhattanDistance(x, y, ti, tj) >= 5;
        };

        auto findNearestOpen = [this](int sx, int sy) {
            std::queue<std::pair<int, int>> q;
            std::vector<std::vector<bool>> visited(N, std::vector<bool>(N, false));
            if (inside(sx, sy) && board[sx][sy] == '.') {
                return std::make_pair(sx, sy);
            }
            if (inside(sx, sy) && !visited[sx][sy]) {
                visited[sx][sy] = true;
                q.emplace(sx, sy);
            }
            while (!q.empty()) {
                auto [x, y] = q.front();
                q.pop();
                for (int dir = 0; dir < 4; ++dir) {
                    int nx = x + dx[dir];
                    int ny = y + dy[dir];
                    if (!inside(nx, ny) || visited[nx][ny]) {
                        continue;
                    }
                    visited[nx][ny] = true;
                    if (board[nx][ny] == '.') {
                        return std::make_pair(nx, ny);
                    }
                    q.emplace(nx, ny);
                }
            }
            return std::make_pair(-1, -1);
        };

        auto [sx, sy] = findNearestOpen(0, 0);
        auto [tx2, ty2] = findNearestOpen(N - 1, N - 1);
        if (sx == -1 || tx2 == -1) {
            return;
        }

        int bottomRow = N - 1;
        int bestJ = -1;
        int bestDiff = N + 1;
        for (int j = 0; j < N; ++j) {
            if (board[bottomRow][j] != '.') {
                continue;
            }
            if (manhattanDistance(bottomRow, j, ti, tj) < 5) {
                continue;
            }
            auto path = shortestPath(si, sj, bottomRow, j, allowFar5);
            if (path.empty()) {
                continue;
            }
            int diff = std::abs(j - sj);
            if (bestJ == -1 || diff < bestDiff || (diff == bestDiff && j < bestJ)) {
                bestJ = j;
                bestDiff = diff;
            }
        }
        if (bestJ == -1) {
            return;
        }
        auto pathStartToEntrance = shortestPath(sx, sy, si, sj, allowAll);
        if (pathStartToEntrance.empty()) {
            return;
        }
        auto pathMiddle = shortestPath(si, sj, bottomRow, bestJ, allowFar2);
        if (pathMiddle.empty()) {
            return;
        }
        auto pathBottomToGoal = shortestPath(bottomRow, bestJ, tx2, ty2, allowAll);
        if (pathBottomToGoal.empty()) {
            return;
        }

        std::vector<std::vector<bool>> onPath(N, std::vector<bool>(N, false));
        for (auto [x, y] : pathMiddle) {
            onPath[x][y] = true;
        }

        auto distFromS = bfsDistance(sx, sy);
        auto distToEntrance = bfsDistance(si, sj);
        auto distFromExit = bfsDistance(bottomRow, bestJ);
        auto distToT = bfsDistance(tx2, ty2);

        auto affectShortest = [&](int x, int y) {
            bool critical = false;
            if (distFromS[si][sj] != -1 && distFromS[x][y] != -1 && distToEntrance[x][y] != -1) {
                if (distFromS[x][y] + distToEntrance[x][y] == distFromS[si][sj]) {
                    critical = true;
                }
            }
            if (distFromExit[tx2][ty2] != -1 && distFromExit[x][y] != -1 && distToT[x][y] != -1) {
                if (distFromExit[x][y] + distToT[x][y] == distFromExit[tx2][ty2]) {
                    critical = true;
                }
            }
            return critical;
        };

        for (auto [vx, vy] : pathMiddle) {
            for (int dir = 0; dir < 4; ++dir) {
                int nx = vx + dx[dir];
                int ny = vy + dy[dir];
                if (!inside(nx, ny)) {
                    continue;
                }
                if (onPath[nx][ny]) {
                    continue;
                }
                if (affectShortest(nx, ny)) {
                    continue;
                }
                tryPlace(nx, ny, pi, pj, placements);
            }
        }
    }

    std::vector<std::pair<int, int>> planTurn(int pi, int pj) {
        std::vector<std::pair<int, int>> placements;
        if (!preprocessed) {
            preprocessInitial(pi, pj, placements);
            preprocessed = true;
        }
        handleAdjacentToFlower(pi, pj, placements);
        handleAdjacentSquares(pi, pj, placements);

        std::vector<int> availableDirs;
        for (int dir = 0; dir < 4; ++dir) {
            int nx = pi + dx[dir];
            int ny = pj + dy[dir];
            if (!inside(nx, ny)) {
                continue;
            }
            if (board[nx][ny] != '.') {
                continue;
            }
            if (confirmed[nx][ny]) {
                continue;
            }
            availableDirs.push_back(dir);
        }

        if (availableDirs.size() == 3) {
            bool hasOppositeVertical = false;
            bool hasOppositeHorizontal = false;
            if (std::find(availableDirs.begin(), availableDirs.end(), 0) != availableDirs.end() &&
                std::find(availableDirs.begin(), availableDirs.end(), 1) != availableDirs.end()) {
                hasOppositeVertical = true;
            }
            if (std::find(availableDirs.begin(), availableDirs.end(), 2) != availableDirs.end() &&
                std::find(availableDirs.begin(), availableDirs.end(), 3) != availableDirs.end()) {
                hasOppositeHorizontal = true;
            }
            if (hasOppositeVertical) {
                placeWithFrontPreference(0, pi, pj, placements, {2, 3}, 2);
                placeWithFrontPreference(1, pi, pj, placements, {2, 3}, 2);
                for (int dir : availableDirs) {
                    if (dir != 0 && dir != 1) {
                        int nx = pi + dx[dir];
                        int ny = pj + dy[dir];
                        tryPlace(nx, ny, pi, pj, placements);
                    }
                }
            } else if (hasOppositeHorizontal) {
                placeWithFrontPreference(2, pi, pj, placements, {2, 3}, 2);
                placeWithFrontPreference(3, pi, pj, placements, {2, 3}, 2);
                for (int dir : availableDirs) {
                    if (dir != 2 && dir != 3) {
                        int nx = pi + dx[dir];
                        int ny = pj + dy[dir];
                        tryPlace(nx, ny, pi, pj, placements);
                    }
                }
            }
        } else if (availableDirs.size() == 2) {
            for (int dir : availableDirs) {
                placeWithFrontPreference(dir, pi, pj, placements, {2, 3}, 2);
            }
        } else if (availableDirs.size() == 1) {
            int dir = availableDirs.front();
            int nx2 = pi + dx[dir] * 2;
            int ny2 = pj + dy[dir] * 2;
            bool blocked = false;
            if (!inside(nx2, ny2)) {
                blocked = true;
            } else if (board[nx2][ny2] == 'T') {
                blocked = true;
            }
            if (!blocked) {
                placeWithFrontPreference(dir, pi, pj, placements, {2, 3}, 3);
            }
        }

        for (int dir = 0; dir < 4; ++dir) {
            handleConfirmedFront(dir, pi, pj, placements);
        }

        blockLineToFlower(pi, pj, placements);
        blockLineToFlowerNeighbors(pi, pj, placements);

        return placements;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Solver solver;
    solver.init();

    while (true) {
        int pi = 0;
        int pj = 0;
        if (!(std::cin >> pi >> pj)) {
            return 0;
        }
        int n = 0;
        std::cin >> n;
        for (int k = 0; k < n; ++k) {
            int x = 0;
            int y = 0;
            std::cin >> x >> y;
            if (solver.inside(x, y)) {
                solver.confirmed[x][y] = true;
            }
        }
        if (pi == solver.ti && pj == solver.tj) {
            break;
        }
        auto placements = solver.planTurn(pi, pj);
        std::cout << placements.size();
        for (auto [x, y] : placements) {
            std::cout << ' ' << x << ' ' << y;
        }
        std::cout << '\n';
        std::cout.flush();
    }
    return 0;
}
