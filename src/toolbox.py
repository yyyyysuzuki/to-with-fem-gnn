from sklearn.metrics import r2_score
import matplotlib.pyplot as plt
import numpy as np
import torch
import json
import csv
import re
import networkx as nx
from torch_geometric.utils import to_networkx

def flatten_list(nested_list):
    flat_list = []
    for item in nested_list:
        if isinstance(item, list):  # アイテムがリストなら、再帰的に展開
            flat_list.extend(flatten_list(item))
        else:
            flat_list.append(item)
    return flat_list

def read_Json(fp):
    with open(fp, 'r', encoding='utf-8') as file:
        data = json.load(file)
    return data

def read_csv(file_path):
    data = []
    with open(file_path, newline='') as file:
        reader = csv.reader(file)  # タブ区切りの場合、delimiter='\t'を使用
        for row in reader:
            data.append([int(value) for value in row])
    return data

def read_csv_float(file_path):
    data = []
    with open(file_path, newline='') as file:
        reader = csv.reader(file)  # タブ区切りの場合、delimiter='\t'を使用
        for row in reader:
            data.append([float(value) for value in row])
    return data


def plot_learning_curve(file_path, loss_values):
    # 学習曲線をプロット
    plt.figure(figsize=(10, 6))
    plt.plot(loss_values, marker='o', linestyle='-', color='b')
    plt.xlabel('epoch')
    plt.ylabel('loss')
    plt.xticks(range(len(loss_values)))  # x軸の目盛りを設定
    plt.grid()

    # グラフをファイルに保存
    plt.savefig(file_path)  # 指定されたファイル名で保存
    plt.close()  # プロットを閉じる

def plot_predictions(true_labels, predicted_values, file_path):
    # R²を計算
    r_squared = r2_score(true_labels, predicted_values)

    # プロット
    plt.figure(figsize=(8, 8))
    plt.scatter(true_labels, predicted_values, color='blue', label='Predicted values')
    plt.plot([min(true_labels), max(true_labels)], [min(true_labels), max(true_labels)], 
             color='red', linestyle='--', label='y=x line')

    # ラベルとタイトルの設定
    plt.xlabel('Prediction')
    plt.ylabel('Labels')
    plt.title(f'Prediction vs True Labels (R² = {r_squared:.2f})')
    plt.legend()
    plt.grid()
    plt.axis('equal')  # x軸とy軸のスケールを同じにする
    plt.xlim(min(true_labels) - 1, max(true_labels) + 1)
    plt.ylim(min(true_labels) - 1, max(true_labels) + 1)

    # プロットをファイルに保存
    plt.savefig(file_path)
    plt.close()  # プロットを閉じる

def write_list_to_csv(lst, filename, header=None):
    with open(filename, "w", newline='') as f:
        writer = csv.writer(f)
        if header:
            writer.writerow([header])  # ヘッダ行（1列）
        for v in lst:
            writer.writerow([v])  # 各要素を1行ずつ書く

def write_two_lists_to_csv(list1, list2, filename, header):
    if len(list1) != len(list2):
        raise ValueError("リストの長さが一致していません")
    
    with open(filename, "w", newline='') as f:
        writer = csv.writer(f)
        if header:
            writer.writerow(header)
        for v1, v2 in zip(list1, list2):
            writer.writerow([v1, v2])

def write_three_lists_to_csv(list1, list2, list3, filename, header):
    if len(list1) != len(list2) or len(list1) != len(list3):
        raise ValueError("リストの長さが一致していません")
    
    with open(filename, "w", newline='') as f:
        writer = csv.writer(f)
        if header:
            writer.writerow(header)
        for v1, v2, v3 in zip(list1, list2, list3):
            writer.writerow([v1, v2, v3])

def write_five_lists_to_csv(list1, list2, list3, list4, list5, filename, header):
    if not (len(list1) == len(list2) == len(list3) == len(list4) == len(list5)):
        raise ValueError("リストの長さが一致していません")
    
    with open(filename, "w", newline='') as f:
        writer = csv.writer(f)
        if header:
            writer.writerow(header)
        for v1, v2, v3, v4, v5 in zip(list1, list2, list3, list4, list5):
            writer.writerow([v1, v2, v3, v4, v5])

def write_six_lists_to_csv(list1, list2, list3, list4, list5, list6, filename, header):
    if not (len(list1) == len(list2) == len(list3) == len(list4) == len(list5) == len(list6)):
        raise ValueError("リストの長さが一致していません")
    
    with open(filename, "w", newline='') as f:
        writer = csv.writer(f)
        if header:
            writer.writerow(header)
        for v1, v2, v3, v4, v5, v6 in zip(list1, list2, list3, list4, list5, list6):
            writer.writerow([v1, v2, v3, v4, v5, v6])

def relative_error_loss_pyg(pred, target, batch, epsilon, max_error=10):
    """
    pred:   [num_nodes, 1] or [num_nodes]
    target: [num_nodes, 1] or [num_nodes]
    batch:  [num_nodes] （各ノードが属するバッチ番号）
    """
    pred = pred.view(-1)
    target = target.view(-1)
    batch = batch.view(-1)

    # 相対誤差を計算（真値が小さすぎる場合の不安定さを回避）
    rel_error = torch.abs(pred - target) / (torch.abs(target) + epsilon)

    # 大きすぎる相対誤差を抑制（例: 外れ値の影響を制限）
    rel_error = torch.clamp(rel_error, min=0.0, max=max_error)

    num_graphs = batch.max().item() + 1
    graph_errors = []

    for i in range(num_graphs):
        mask = (batch == i)
        if mask.sum() > 0:
            mean_error = rel_error[mask].mean()
            graph_errors.append(mean_error)
    
    return torch.stack(graph_errors).mean(),graph_errors

def relative_loss(y_pred, y_true, epsilon=1e-15, max_rel_error=10.0):
    """
    y_pred: 予測値 (Tensor)
    y_true: 真値 (Tensor)
    epsilon: 真値が0に近い時の除算安定化項
    max_rel_error: 相対誤差の上限値（clamp）
    """

    # 相対誤差を計算（真値が小さすぎる場合の不安定さを回避）
    rel_error = torch.abs(y_pred - y_true) / (torch.abs(y_true) + epsilon)

    # 大きすぎる相対誤差を抑制（例: 外れ値の影響を制限）
    rel_error = torch.clamp(rel_error, min=0.0, max=max_rel_error)

    # 平均誤差として出力
    return rel_error.mean()

def mse_loss_pyg(pred, target, batch):
    """
    pred:   [num_nodes, 1] or [num_nodes]
    target: [num_nodes, 1] or [num_nodes]
    batch:  [num_nodes] （各ノードが属するバッチ番号）
    """
    pred = pred.view(-1)
    target = target.view(-1)
    batch = batch.view(-1)

    squared_error = (pred - target) ** 2

    num_graphs = batch.max().item() + 1
    graph_errors = []

    for i in range(num_graphs):
        mask = (batch == i)
        if mask.sum() > 0:
            mean_error = squared_error[mask].mean()
            graph_errors.append(mean_error)
    
    return torch.stack(graph_errors).mean(), graph_errors

def CalB_v2(A, NodeData, ElementData):
    e = ElementData[:, 1:4].astype(int)   # (nElem, 3)
    x = NodeData[e, 0]  # (nElem, 3)
    y = NodeData[e, 1]  # (nElem, 3)

    # j,k の組み合わせで差をとる
    c = np.stack([y[:,1]-y[:,2], y[:,2]-y[:,0], y[:,0]-y[:,1]], axis=1)
    d = np.stack([x[:,2]-x[:,1], x[:,0]-x[:,2], x[:,1]-x[:,0]], axis=1)

    delta = 0.5*(c[:,0]*d[:,1] - c[:,1]*d[:,0])  # (nElem,)

    A_e = A[e]  # (nElem, 3)
    A_e = A_e.squeeze(-1)

    Bx = 0.5*np.sum(d * A_e, axis=1) / delta
    By = -0.5*np.sum(c * A_e, axis=1) / delta

    return Bx, By


def Paraview_Bvector_Acontour(filename, A, Bx, By, NodeData, ElementData):
    with open(filename, 'w') as f:
        TotalNodeNumber = NodeData.shape[0]
        TotalElementNumber = ElementData.shape[0]

        f.write("# vtk DataFile Version 2.0\n")
        f.write("Title Data\n")
        f.write("ASCII\n")
        f.write("DATASET UNSTRUCTURED_GRID\n")

        # 節点座標出力
        f.write(f"POINTS {TotalNodeNumber} float\n")
        for i in range(TotalNodeNumber):
            x, y = NodeData[i][0], NodeData[i][1]
            z = NodeData[i][2] if NodeData.shape[1] > 2 else 0.0
            f.write(f"{x:.16f} {y:.16f} {z:.16f}\n")

        # セル情報出力（三角形）
        f.write(f"CELLS {TotalElementNumber} {4 * TotalElementNumber}\n")
        for i in range(TotalElementNumber):
            n1, n2, n3 = ElementData[i, 1:4]
            f.write(f"3 {n1} {n2} {n3}\n")

        # セルタイプ（5は三角形）
        f.write(f"CELL_TYPES {TotalElementNumber}\n")
        f.writelines("5\n" for _ in range(TotalElementNumber))

        # スカラー出力（節点ごとのA）
        f.write(f"POINT_DATA {TotalNodeNumber}\n")
        f.write("SCALARS A float\n")
        f.write("LOOKUP_TABLE default\n")
        for val in A:
            f.write(f"{val.item():.16f}\n")

        # ベクトル出力（要素ごとのBx, By）
        f.write(f"CELL_DATA {TotalElementNumber}\n")
        f.write("VECTORS BxBy float\n")
        for i in range(TotalElementNumber):
            f.write(f"{Bx[i].item():.16f} {By[i].item():.16f} 0.0\n")

def Paraview_Acontour(filename, Potential, NodeData, ElementData):
    with open(filename, "w") as fp:
        numnode = NodeData.shape[0]
        numele = ElementData.shape[0]

        fp.write("# vtk DataFile Version 2.0\n")
        fp.write("material\n")
        fp.write("ASCII\n")
        fp.write("DATASET UNSTRUCTURED_GRID\n")

        # POINTS
        fp.write(f"POINTS {numnode} float\n")
        for i in range(numnode):
            fp.write(f"{NodeData[i][0]:.15f} {NodeData[i][1]:.15f} {NodeData[i][2]:.15f}\n")

        # CELLS
        fp.write(f"CELLS {numele} {4 * numele}\n")
        for i in range(numele):
            fp.write(f"3 {ElementData[i][1]} {ElementData[i][2]} {ElementData[i][3]}\n")

        # CELL_TYPES
        fp.write(f"CELL_TYPES {numele}\n")
        for _ in range(numele):
            fp.write("5\n")  # 5 = triangle

        # POINT_DATA
        fp.write(f"POINT_DATA {numnode}\n")
        fp.write("SCALARS point_data float\n")
        fp.write("LOOKUP_TABLE default\n")
        for i in range(numnode):
            fp.write(f"{float(Potential[i]):.15f}\n")

def plot_pyg_graph_2(
    data, pos_list, node_values, vmin, vmax,
    save_path="graph.png",
    node_size=200,
    label_fontsize=6,
    cbar_fontsize=10,
):
    """
    PyGのData型をnetworkxグラフに変換し、ノード値に基づくカラーマップ付きで可視化・保存する関数。
    """

    # NetworkXグラフに変換
    G = to_networkx(data, to_undirected=True)

    # カラーマップ用データ
    node_values = np.array(node_values)

    plt.figure(figsize=(10, 8))  # 30x25 はかなり大きいので少し小さめに

    # グラフ本体の描画
    nx.draw(
        G,
        pos=pos_list,
        node_color=node_values,
        cmap=plt.cm.jet,
        vmin=vmin,
        vmax=vmax,
        with_labels=False,   # ラベルは後で別に描画
        node_size=node_size,
        edge_color="gray",
        linewidths=0.5
    )

    # ノードラベルの描画
    nx.draw_networkx_labels(
        G,
        pos_list,
        font_size=label_fontsize,
        font_color="black"
    )

    # カラーバーの設定（小さく＆フォント大きめ）
    sm = plt.cm.ScalarMappable(cmap=plt.cm.jet,
                               norm=plt.Normalize(vmin=vmin, vmax=vmax))
    sm.set_array([])

    cbar = plt.colorbar(
        sm,
        fraction=0.035,  # カラーバーの太さ・長さ調整
        pad=0.02,        # 本体との隙間
        aspect=30        # アスペクト比
    )
    cbar.ax.tick_params(labelsize=cbar_fontsize)

    # 軸は消しておくとすっきり
    plt.axis("off")
    plt.tight_layout()
    plt.savefig(save_path, dpi=300)
    plt.close()

def plot_pyg_graph(data, pos_list, node_values, vmin, vmax, save_path="graph.png"):
    """
    PyGのData型をnetworkxグラフに変換し、ノード値に基づくカラーマップ付きで可視化・保存する関数。

    Parameters
    ----------
    data : torch_geometric.data.Data
        PyGのグラフデータ
    pos_list : list or np.ndarray
        ノードの位置座標 [(x0, y0), (x1, y1), ...]
    node_values : list or np.ndarray
        ノードごとのスカラー値
    vmin : float
        カラーマップの最小値
    vmax : float
        カラーマップの最大値
    save_path : str
        画像を保存するパス
    """
    # NetworkXグラフに変換
    G = to_networkx(data, to_undirected=True)

    # カラーマップ用データ
    node_values = np.array(node_values)

    # 描画
    plt.figure(figsize=(30, 25))
    nx.draw(
        G,
        pos=pos_list,
        node_color=node_values,
        cmap=plt.cm.jet,
        vmin=vmin,
        vmax=vmax,
        with_labels=True,
        node_size=400,
        edge_color="gray"
    )
    nx.draw_networkx_labels(G, pos_list, font_size=0.5, font_color="black")
    plt.colorbar(plt.cm.ScalarMappable(cmap=plt.cm.jet, norm=plt.Normalize(vmin=vmin, vmax=vmax)))
    plt.savefig(save_path, dpi=300)
    plt.close()

def cal_gravity(NodeData, ElementData):
    node_indices = ElementData[:, 1:4].astype(int)

    # 各要素のノード座標を取得 (num_elements, 3, 2)
    coords = NodeData[node_indices]

    # 平均を取って重心座標を求める
    gravity = coords.mean(axis=1)  # (num_elements, 2)

    Gx, Gy = gravity[:, 0], gravity[:, 1]
    return Gx, Gy

def precompute_G0(Gx, Gy, mask, gaussian_centers, dtype=np.float64):
    """
    Gx, Gy            : (Ne,)
    mask              : (Ne,) bool（True=設計領域）
    gaussian_centers  : (M,2)  [mu_x, mu_y]
    gaussian_sigma    : (M,)   [sigma_j]
    戻り値:
      G0  : (Ne_mask, M)
      den : (Ne_mask,)  行和（0割防止付き）
    """
    Gx_m = Gx[mask].astype(dtype)[:, None]     # (Ne_mask,1)
    Gy_m = Gy[mask].astype(dtype)[:, None]     # (Ne_mask,1)
    mu_x = gaussian_centers[:, 0].astype(dtype)[None, :]  # (1,M)
    mu_y = gaussian_centers[:, 1].astype(dtype)[None, :]  # (1,M)
    sig  = gaussian_centers[:, 2].astype(dtype)[None, :]  # (1,M)

    dx2  = (Gx_m - mu_x)**2
    dy2  = (Gy_m - mu_y)**2
    sig2 = sig**2

    # あなたの Cal_G と同じ正規化（1/(2πσ)）
    G0 = (1.0/(2.0*np.pi*sig)) * np.exp(-(dx2 + dy2)/(2.0*sig2))   # (Ne_mask, M)

    den = G0.sum(axis=1)
    den[den == 0.0] = 1.0  # 0割防止
    return G0, den

def write_material_vtk(filename: str,
                       nodes: np.ndarray,
                       elements: np.ndarray,
                       material_map: dict | None = None):
    """
    nodes: shape (N,2) or (N,3) の float 配列（[x,y,(z)]）
    elements: shape (M,4) の int 配列（[material_id, n1, n2, n3]）※節点番号は0始まり
    material_map: { 元の材料ID:int -> 出力スカラー:int } の辞書
                  例: {airNumber:0, ironNumber:1, magnetNumber:2}
                  未指定/辞書にないIDは 3 を出力

    出力は VTK Legacy (ASCII) UNSTRUCTURED_GRID（全セル=三角形, CELL_TYPE=5）。
    """
    # ---- 入力の正規化 ----
    nodes = np.asarray(nodes)
    if nodes.ndim != 2 or nodes.shape[1] not in (2, 3):
        raise ValueError("nodes は shape (N,2) または (N,3) である必要があります。")
    if nodes.shape[1] == 2:
        # z=0 を付与
        nodes = np.c_[nodes, np.zeros((nodes.shape[0],), dtype=nodes.dtype)]

    elements = np.asarray(elements, dtype=np.int64)
    if elements.ndim != 2 or elements.shape[1] != 4:
        raise ValueError("elements は shape (M,4)（[mat, n1, n2, n3]）である必要があります。")

    n_nodes = nodes.shape[0]
    n_cells = elements.shape[0]

    # ---- CELLS セクション用（各行: 3 n1 n2 n3）----
    # 総整数数 = M * (3+1) = 4*M
    # 先頭列はすべて"3"（三角形の頂点数）
    cells_block = np.column_stack([
        np.full((n_cells,), 3, dtype=np.int64),
        elements[:, 1:4]
    ])

    # ---- CELL_TYPES（全て VTK_TRIANGLE=5）----
    cell_types = np.full((n_cells, 1), 5, dtype=np.int32)

    # ---- CELL_DATA（材料スカラーを書き出し）----
    # 既定値 3（その他）
    scalars = np.full((n_cells,), 3, dtype=np.int32)
    if material_map:
        # 登録された ID は対応スカラーに置換
        for src_id, out_val in material_map.items():
            mask = (elements[:, 0] == src_id)
            if np.any(mask):
                scalars[mask] = int(out_val)

    # ---- 書き出し ----
    with open(filename, "w", encoding="ascii") as f:
        f.write("# vtk DataFile Version 2.0\n")
        f.write("Title Data\n")
        f.write("ASCII\n")
        f.write("DATASET UNSTRUCTURED_GRID\n")

        # POINTS
        f.write(f"POINTS {n_nodes} float\n")
        # 高速に3列を書き出し
        np.savetxt(f, nodes, fmt="%.16g %.16g %.16g")

        # CELLS
        f.write(f"CELLS {n_cells} {n_cells * 4}\n")
        np.savetxt(f, cells_block, fmt="%d %d %d %d")

        # CELL_TYPES
        f.write(f"CELL_TYPES {n_cells}\n")
        np.savetxt(f, cell_types, fmt="%d")

        # CELL_DATA + SCALARS
        f.write(f"CELL_DATA {n_cells}\n")
        f.write("SCALARS Magnetic_Flux_Density int\n")
        f.write("LOOKUP_TABLE default\n")
        np.savetxt(f, scalars.reshape(-1, 1), fmt="%d")

def get_onehot(material_list_with_nodei):
    unique = set(material_list_with_nodei)
    if unique == {0}:
        return [1,0,0,0,0]
    elif unique == {1}:
        return [0,1,0,0,0]
    elif unique == {2}:
        return [0,0,1,0,0]
    elif unique == {0,1}:
        return [0,0,0,1,0]
    elif unique == {1,2}:
        return [0,0,0,0,1]
    else:
        print("未知の材料番号です")
        exit(1)

def get_i_from_id(id_name, local_indnum):
    # 正規表現で "ind_" の後ろの数字を取得
    m = re.search(r'ind_(\d+)', id_name)
    if not m:
        raise ValueError(f"フォーマットが合わない: {id_name}")
    ind_num = int(m.group(1))   # 個体番号
    i = ind_num - local_indnum
    return i, ind_num

def CalTargetBB(Bx: np.ndarray, By: np.ndarray):
    # C++側は1始まりなので Pythonでは -1 する
    target = [2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020]

    ABSB = 0.0
    for i in target:
        BB = np.sqrt(Bx[i-1]**2 + By[i-1]**2)
        #print("BB :", BB)
        ABSB += BB

    #print("ABSB :", ABSB)
    return ABSB

def cal_delta(NodeData: np.ndarray, ElementData: np.ndarray):
    """
    NodeData: shape = (N_nodes, 2) or (N_nodes, >=2)
        各ノードの [x, y] 座標
    ElementData: shape = (N_elements, 4)
        各行は [Material, n1, n2, n3]
    return:
        delta: shape = (N_elements,)
        各要素（三角形）の面積
    """

    # ノードインデックスを抽出（Material列を除く）
    elems = ElementData[:, 1:4].astype(int)  # (E, 3)

    # 各要素のノード座標を取得
    coords = NodeData[elems, :2]  # (E, 3, 2) = 各要素の3点の(x, y)

    # ノードを a, b, c としたとき:
    a = coords[:, 0, :]  # (E, 2)
    b = coords[:, 1, :]  # (E, 2)
    c = coords[:, 2, :]  # (E, 2)

    # ベクトルAB, AC
    ab = b - a
    ac = c - a

    # 外積のz成分の絶対値の1/2が面積
    delta = 0.5 * np.abs(ab[:, 0] * ac[:, 1] - ab[:, 1] * ac[:, 0])

    # 面積ゼロまたは負の要素をチェック
    if np.any(delta <= 0):
        raise ValueError("要素面積がゼロまたは負の値です。")

    return delta

def get_bin_index(srore: float, bins: list) -> int:
    """
    評価値 score が属する区間 (0~7) を返す
    """
    for idx, (lower, upper) in enumerate(bins):
        if lower <= srore < upper:
            return idx
    
    return len(bins) - 1