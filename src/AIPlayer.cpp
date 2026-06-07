#include "AIPlayer.hpp"
#include "Board.hpp"
#include <limits>
#include <algorithm>
#include <random>

AIPlayer::AIPlayer(int id, const std::string& name, int depth): Player(id, name), maxDepth(depth) {}

int AIPlayer::getMove(const Board& boardConst) {
    Board board = boardConst; // make a local copy to simulate
    auto moves = board.validMoves();
    if (moves.empty()) return -1;
    int bestScore = std::numeric_limits<int>::min();
    int bestMove = moves[0];
    for (int m : moves) {
        board.dropDisc(m, getId());
        int score = minimax(board, 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        board.undoDrop(m);
        if (score > bestScore) { bestScore = score; bestMove = m; }
    }
    return bestMove;
}

int AIPlayer::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    int winner = board.checkWin();
    if (winner == getId()) return 100000 - depth;
    if (winner != 0 && winner != getId()) return -100000 + depth;
    if (board.isFull() || depth >= maxDepth) return evaluate(board);

    auto moves = board.validMoves();
    if (maximizingPlayer) {
        int value = std::numeric_limits<int>::min();
        for (int m : moves) {
            board.dropDisc(m, getId());
            value = std::max(value, minimax(board, depth+1, alpha, beta, false));
            board.undoDrop(m);
            alpha = std::max(alpha, value);
            if (alpha >= beta) break;
        }
        return value;
    } else {
        int opp = (getId() == 1) ? 2 : 1;
        int value = std::numeric_limits<int>::max();
        for (int m : moves) {
            board.dropDisc(m, opp);
            value = std::min(value, minimax(board, depth+1, alpha, beta, true));
            board.undoDrop(m);
            beta = std::min(beta, value);
            if (alpha >= beta) break;
        }
        return value;
    }
}

int AIPlayer::evaluate(const Board& board) {
    // Simple heuristic: center column control + two/three in a row
    int score = 0;
    // center control
    int center = COLS/2;
    for (int r = 0; r < ROWS; ++r) if (board.at(r, center) == getId()) score += 3;
    // count two/three sequences for player and subtract opponent
    auto countSeq = [&](int player, int len){
        int cnt = 0;
        // horizontal
        for (int r = 0; r < ROWS; ++r) for (int c = 0; c <= COLS - len; ++c) {
            int k; int i;
            for (k = 0; k < len; ++k) if (board.at(r,c+k) != player && board.at(r,c+k) != 0) break;
            if (k==len) cnt++;
        }
        // vertical
        for (int c = 0; c < COLS; ++c) for (int r = 0; r <= ROWS - len; ++r) {
            int k; for (k = 0; k < len; ++k) if (board.at(r+k,c) != player && board.at(r+k,c) != 0) break;
            if (k==len) cnt++;
        }
        // diag down-right
        for (int r = 0; r <= ROWS - len; ++r) for (int c = 0; c <= COLS - len; ++c) {
            int k; for (k=0;k<len;++k) if (board.at(r+k,c+k) != player && board.at(r+k,c+k) != 0) break;
            if (k==len) cnt++;
        }
        // diag up-right
        for (int r = len-1; r < ROWS; ++r) for (int c = 0; c <= COLS - len; ++c) {
            int k; for (k=0;k<len;++k) if (board.at(r-k,c+k) != player && board.at(r-k,c+k) != 0) break;
            if (k==len) cnt++;
        }
        return cnt;
    };
    int my2 = countSeq(getId(), 2);
    int my3 = countSeq(getId(), 3);
    int opp = (getId()==1)?2:1;
    int op2 = countSeq(opp,2);
    int op3 = countSeq(opp,3);
    score += my2*10 + my3*100;
    score -= op2*9 + op3*90;
    return score;
}
