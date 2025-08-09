from collections import defaultdict


def parse_time(s):
    s = s.strip()
    if not s or s == "-" or s.upper() == "NULL":
        return "00:00"
    if ":" in s:
        return s.zfill(5)
    if len(s) == 4:
        return s[:2] + ":" + s[2:]
    if len(s) == 3:
        return "0" + s[0] + ":" + s[1:]
    return "00:00"


def parse_price(s):
    if not s or s == "-" or s.upper() == "NULL":
        return 40.00
    try:
        return float(s)
    except:
        return 40.00


def main():
    input_path = "../data/TrainInfo.tbl"
    output_path = "../data/TrainInfo_parsed.txt"
    trains = defaultdict(list)
    with open(input_path, encoding="utf-8") as f:
        for line in f:
            row = [x.strip() for x in line.strip().split("|")]
            if not row or len(row) < 11:
                continue
            train_id = row[0]
            start = row[2]
            end = row[3]
            depart = parse_time(row[5])
            arrive = parse_time(row[6])
            acc_price = parse_price(row[10])
            trains[train_id].append((start, end, depart, arrive, acc_price))
    with open(output_path, "w", encoding="utf-8") as fout:
        for train_id, segs in trains.items():
            station_order = [segs[0][0]]
            for seg in segs:
                station_order.append(seg[1])
            n = len(station_order)
            fout.write(f"{train_id} {n}\n")
            fout.write(" ".join(station_order) + "\n")
            last_acc = None
            for seg in segs:
                if last_acc is None or seg[4] == 40.00 or last_acc == 40.00:
                    price = seg[4]
                else:
                    price = seg[4] - last_acc
                if price < 0:
                    price = 40.00
                last_acc = seg[4]
                fout.write(f"{seg[2]} {seg[3]} {price:.2f}\n")
            fout.write("\n")


if __name__ == "__main__":
    main()
