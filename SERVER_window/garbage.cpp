
// ---- L_CAHT.cpp

// �̹��� ���� ������
void Thread_Recv_UDP(int port, int index) {
	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr.Get_user_idx(index);
	//SOCKET sock = Initialize(port, 2);
	SOCKET sock = Initialize(port, 1);
	Mat rawData;
	Mat frame;


	listen(sock, 1);
	SOCKADDR_IN adr;
	memset(&adr, 0, sizeof(adr));
	int adr_sz = sizeof(adr);

	SOCKET sock_ = accept(sock, (SOCKADDR*)&adr, &adr_sz);

	char* longbuf;
	char buffer[BUF_LEN];
	int recvMsgSize;
	int total_pack;
	int max_sz;

	while (1) {
		try {
			do {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
			} while (recvMsgSize > sizeof(int));
			total_pack = ((int*)buffer)[0];

			longbuf = new char[PACK_SIZE * total_pack];

			/*
			for (int i = 0; i < total_pack; i++) {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				if (recvMsgSize != PACK_SIZE) {
					//cerr << "Received unexpected size pack:" << recvMsgSize << endl;
					continue;
				}
				memcpy(&longbuf[i * PACK_SIZE], buffer, recvMsgSize);
			}*/

			max_sz = 0;
			while (1) {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				cout << "recv msg : <" << recvMsgSize << "/" << total_pack * PACK_SIZE << ">" << endl;
				memcpy(&longbuf[max_sz], buffer, recvMsgSize);

				max_sz += recvMsgSize;
				if (max_sz == total_pack * PACK_SIZE)
					break;
				cout << "�߰� ������ \n";
			}


			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			//MATS[index] = rawData;

			frame = imdecode(rawData, IMREAD_COLOR);

			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "������ ������ Ʋ��" << endl;
				continue;
			}

			user->lock.write_lock();
			memcpy(&MATS[index], &rawData, sizeof(Mat));
			user->lock.write_unlock();

			//ttt = frame;

			isSAVE[index] = true;
			free(longbuf);
			//waitKey(1000 / 30);
		}
		catch (exception e) {
			cout << "recv thread error!" << endl;
		}
	}
	closesocket(sock);
	printf("[RECEIVER] Thread END<%d>\n", port);
}
// �̹��� �۽� ������
void Thread_Send_UDP(int port, int index) {
	printf("[SENDER] Thread START<%d / %d>\n", port, index);
	USER* user = &cmd_ctr.Get_user_idx(index);
	SOCKET sock = Initialize(port, 2);
	SOCKADDR_IN target_adr;
	memset(&target_adr, 0, sizeof(target_adr));

	char msg[BUF_SIZE];
	int msg_len;
	int client_adr_sz;
	client_adr_sz = sizeof(target_adr);
	msg_len = recvfrom(sock, msg, BUF_SIZE, 0, (struct sockaddr*) & target_adr, &client_adr_sz);
	msg[msg_len] = 0;
	printf("[SENDER]recv message :				%s\n", msg);

	printf("[SENDER] waiting connect!\n");
	connect(sock, (struct sockaddr*) & target_adr, sizeof(target_adr));
	printf("[SENDER] connect done!\n");

	int jpegqual = ENCODE_QUALITY;
	Mat temp;

	vector<uchar>encoded;
	vector < int > compression_params;
	int ibuf[1];
	int total_pack;

	while (1) {
		if (isSAVE[index] == false) {
			//cout << "img does not saved["<<index<<"]"<<endl;
			continue;
		}
		try {

			user->lock.read_lock();
			Mat frame = imdecode(MATS[index], IMREAD_COLOR);
			user->lock.read_unlock();

			if (frame.size().width == 0) {
				////cerr << "decode fadilure!" << endl;
				continue;
			}



			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);
			imencode(".jpg", frame, encoded, compression_params);
			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;


			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			send(sock, (char*)ibuf, sizeof(int), 0);
			//puts("packet size send");
			// ������ ��Ŷ �� ��ŭ ������ ����
			for (int i = 0; i < total_pack; i++) {
				send(sock, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
				//printf("send packet data %d\n", i);
			}
			waitKey(1000 / 20);
		}
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}

	}
	printf("[SENDER] Thread END<%d>\n", port);
}